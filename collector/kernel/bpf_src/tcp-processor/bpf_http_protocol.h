//
// Copyright 2021 Splunk Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

//
// bpf_http_request.h - BPF HTTP Request Parsing
//

#pragma once 

struct http_protocol_state_data_t {
  u64 request_timestamp;
  u64 __unused; // Keep this to TCP_SOCKET_PROTOCOL_STATE_SIZE
};

static enum TCP_PROTOCOL_DETECT_RESULT
http_detect(struct pt_regs *ctx, struct tcp_connection_t *pconn,
            struct tcp_control_value_t *pctrl, enum STREAM_TYPE streamtype,
            const u8 *data, size_t data_len)
{
#if TRACE_HTTP_PROTOCOL
  DEBUG_PRINTK("http_detect: start=%llu, total=%llu\n",
               (u32)pctrl->streams[(int)streamtype].start,
               pconn->streams[(int)streamtype].total);
  DEBUG_PRINTK("             data=%s, data_len=%d\n", data, (int)data_len);
#endif

  enum TCP_PROTOCOL_DETECT_RESULT res = TPD_UNKNOWN;

  // Does the buffer start with an HTTP verb?
  if (data_len >= 3) {

    char c = 0;
    bpf_probe_read(&c, 1, data);
    switch (c) {
    case 'G':
      res = string_starts_with(data + 1, data_len - 1, "ET") ? TPD_SUCCESS
                                                             : TPD_FAILED;
      break;
    case 'H':
      if (data_len >= 4) {
        res = string_starts_with(data + 1, data_len - 1, "EAD") ? TPD_SUCCESS
                                                                : TPD_FAILED;
      }
      break;
    case 'D':
      if (data_len >= 6) {
        res = string_starts_with(data + 1, data_len - 1, "ELETE") ? TPD_SUCCESS
                                                                  : TPD_FAILED;
      }
      break;
    case 'C':
      if (data_len >= 7) {
        res = string_starts_with(data + 1, data_len - 1, "ONNECT")
                  ? TPD_SUCCESS
                  : TPD_FAILED;
      }
      break;
    case 'O':
      if (data_len >= 7) {
        res = string_starts_with(data + 1, data_len - 1, "PTIONS")
                  ? TPD_SUCCESS
                  : TPD_FAILED;
      }
      break;
    case 'T':
      if (data_len >= 5) {
        res = string_starts_with(data + 1, data_len - 1, "RACE") ? TPD_SUCCESS
                                                                 : TPD_FAILED;
      }
      break;

    case 'P':
      bpf_probe_read(&c, 1, data + 1);
      switch (c) {
      case 'U':
        res = string_starts_with(data + 2, data_len - 2, "T") ? TPD_SUCCESS
                                                              : TPD_FAILED;
        break;
      case 'O':
        res = string_starts_with(data + 2, data_len - 2, "ST") ? TPD_SUCCESS
                                                               : TPD_FAILED;
        break;
      case 'A':
        if (data_len >= 5) {
          res = string_starts_with(data + 2, data_len - 2, "TCH") ? TPD_SUCCESS
                                                                  : TPD_FAILED;
        }
        break;
      default:
        res = TPD_FAILED;
        break;
      }
      break;

    default:
      res = TPD_FAILED;
      break;
    }
  }

  return res;
}

static void http_process_request(struct pt_regs *ctx,
                                 struct tcp_connection_t *pconn,
                                 struct tcp_control_value_t *pctrl,
                                 enum STREAM_TYPE streamtype, const u8 *data,
                                 size_t data_len)
{
#if TRACE_HTTP_PROTOCOL
  DEBUG_PRINTK("http_process_request: start=%llu, total=%llu\n",
               (u32)pctrl->streams[(int)streamtype].start,
               pconn->streams[(int)streamtype].total);
  DEBUG_PRINTK("             data=%s, data_len=%d\n", data, (int)data_len);
#endif

  // Keep the request timestamp for latency calculation
  struct http_protocol_state_data_t *state_data =
      (struct http_protocol_state_data_t *)(pconn->protocol_state.data);
  state_data->request_timestamp = get_timestamp();

  // Enable getting the response, and turn off the request side until we get it
  if (streamtype == ST_RECV) {
    enable_tcp_connection(pctrl, -1, 1);
  } else {
    enable_tcp_connection(pctrl, 1, -1);
  }
}

static void http_process_response(struct pt_regs *ctx,
                                  struct tcp_connection_t *pconn,
                                  struct tcp_control_value_t *pctrl,
                                  enum STREAM_TYPE streamtype, const u8 *data,
                                  size_t data_len)
{
#if TRACE_HTTP_PROTOCOL
  DEBUG_PRINTK("http_process_response: start=%llu, total=%llu\n",
               (u32)pctrl->streams[(int)streamtype].start,
               pconn->streams[(int)streamtype].total);
  DEBUG_PRINTK("             data=%s, data_len=%d\n", data, (int)data_len);
#endif

  // HTTP response can't be less than 12 characters long, if we haven't gotten
  // 12 characters yet, just wait until more bytes show up
  if (data_len < 12) {
    goto finished_response;
  }

  char localdata[12] = {};
  bpf_probe_read(localdata, 12, data);

  // Pattern match against HTTP/x.y
  if (!string_starts_with(localdata, 12, "HTTP/") || localdata[6] != '.' ||
      localdata[8] != ' ') {
    goto finished_response;
  }
  int http_version_major = char_to_number(localdata[5]);
  int http_version_minor = char_to_number(localdata[7]);

#if TRACE_HTTP_PROTOCOL
  DEBUG_PRINTK("HTTP version: %d.%d\n", http_version_major, http_version_minor);
#endif
  // Check for invalid HTTP version
  if (http_version_major == -1 || http_version_minor == -1) {
    goto finished_response;
  }

  // Convert HTTP response code to integer
  int http_code_2 = char_to_number(localdata[9]);
  int http_code_1 = char_to_number(localdata[10]);
  int http_code_0 = char_to_number(localdata[11]);

#if TRACE_HTTP_PROTOCOL
  DEBUG_PRINTK("HTTP response code: %d%d%d\n", http_code_2, http_code_1,
               http_code_0);
#endif

  // Ensure each digit of HTTP response code is valid
  if (http_code_2 == -1 || http_code_1 == -1 || http_code_0 == -1) {
    goto finished_response;
  }

  u16 code = (u16)(http_code_2 * 100 + http_code_1 * 10 + http_code_0);

  // Submit the http response code, and latency
  struct http_protocol_state_data_t *state_data =
      (struct http_protocol_state_data_t *)(pconn->protocol_state.data);
  u64 latency = get_timestamp() - state_data->request_timestamp;
  u8 client_server = (streamtype == ST_SEND) ? SC_SERVER : SC_CLIENT;
  tcp_events_submit_http_response(ctx, pconn->sk, code, latency, client_server);

  // Enable getting the request, and turn off the response side until we get it
finished_response:

  if (streamtype == ST_RECV) {
    enable_tcp_connection(pctrl, -1, 1);
  } else {
    enable_tcp_connection(pctrl, 1, -1);
  }
}
