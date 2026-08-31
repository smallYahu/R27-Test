#include "en_dc.h"
#include <stdlib.h>

/*****************************************************************************
 * Defines
 ****************************************************************************/

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

/*****************************************************************************
 * Functions
 ****************************************************************************/

/* Encode 
 * TODO : 
 * - Check the encoding loop and the buffer sizes make sure matches the required ones to the algorithm used 
 * - Check the condition based on which the whole encoding process takes place 
 * */
encode_result frame_encode(void *dst_buf_ptr, size_t dst_buf_len, const void *src_ptr, size_t src_len) {
  encode_result result = {0u, ENCODE_OK};
  if ((dst_buf_ptr == NULL) || (src_ptr == NULL)) {
    result.status = ENCODE_NULL_POINTER;
    return result;
  }
  const uint8_t *src_read_ptr = (const uint8_t *)src_ptr;
  const uint8_t *src_end_ptr = src_read_ptr + src_len;
  uint8_t *dst_buf_start_ptr = (uint8_t *)dst_buf_ptr;
  uint8_t *dst_buf_end_ptr = dst_buf_start_ptr + dst_buf_len;
  uint8_t *dst_code_write_ptr = dst_buf_start_ptr;
  uint8_t *dst_write_ptr = dst_buf_start_ptr + 1u;
  uint8_t code = 1u;
  for (size_t i = 0u; i < src_len; i++) {
    if (dst_code_write_ptr >= dst_buf_end_ptr) {
      result.status |= ENCODE_OUT_BUFFER_OVERFLOW;
      break;
    }
    uint8_t src_byte = *src_read_ptr++;
    if (src_byte == 0u) {
      *dst_code_write_ptr = code;
      dst_code_write_ptr = dst_write_ptr++;
      code = 1u;
    } else {
      if (dst_write_ptr >= dst_buf_end_ptr) {
        result.status |= ENCODE_OUT_BUFFER_OVERFLOW;
        break;
      }
      *dst_write_ptr++ = src_byte;
      code++;
      if (code == 0xFFu) {
        *dst_code_write_ptr = code;
        dst_code_write_ptr = dst_write_ptr++;
        code = 1u;
      }
    }
  }
  if (dst_code_write_ptr >= dst_buf_end_ptr) {
    result.status |= ENCODE_OUT_BUFFER_OVERFLOW;
    dst_write_ptr = dst_buf_end_ptr;
  } else {
    *dst_code_write_ptr = code;
  }
  result.out_len = (size_t)(dst_write_ptr - dst_buf_start_ptr);
  return result;
}

/* Decode 
 * TODO:
 * - Verify that the decoding loop processes the complete input stream.
 * - Add appropriate termination logic for the decoding process.
 * - Ensure the decoder does not read beyond the input buffer.
 * */
decode_result frame_decode(void *dst_buf_ptr, size_t dst_buf_len, const void *src_ptr, size_t src_len) {
  decode_result result = {0u, DECODE_OK};
  if ((dst_buf_ptr == NULL) || (src_ptr == NULL)) {
    result.status = DECODE_NULL_POINTER;
    return result;
  }
  const uint8_t *src_read_ptr = (const uint8_t *)src_ptr;
  const uint8_t *src_end_ptr = src_read_ptr + src_len;
  uint8_t *dst_buf_start_ptr = (uint8_t *)dst_buf_ptr;
  uint8_t *dst_buf_end_ptr = dst_buf_start_ptr + dst_buf_len;
  uint8_t *dst_write_ptr = dst_buf_start_ptr;
  while (src_read_ptr < src_end_ptr) {
    uint8_t len_code = *src_read_ptr++;
    if (len_code == 0u) {
      result.status |= DECODE_ZERO_BYTE_IN_INPUT;
      break;
    }
    uint8_t copy_len = len_code - 1u;
    if ((size_t)(src_end_ptr - src_read_ptr) < copy_len) {
      result.status |= DECODE_INPUT_TOO_SHORT;
      copy_len = (uint8_t)(src_end_ptr - src_read_ptr);
    }
    if ((size_t)(dst_buf_end_ptr - dst_write_ptr) < copy_len) {
      result.status |= DECODE_OUT_BUFFER_OVERFLOW;
      copy_len = (uint8_t)(dst_buf_end_ptr - dst_write_ptr);
    }
    for (uint8_t i = 0u; i < copy_len; i++) {
      uint8_t src_byte = *src_read_ptr++;
      if (src_byte == 0u) {
        result.status |= DECODE_ZERO_BYTE_IN_INPUT;
      }
      *dst_write_ptr++ = src_byte;
    }
    if (len_code < 0xFFu && src_read_ptr < src_end_ptr) {
      if (dst_write_ptr < dst_buf_end_ptr) {
        *dst_write_ptr++ = 0u;
      } else {
        result.status |= DECODE_OUT_BUFFER_OVERFLOW;
      }
    }
  }
  result.out_len = (size_t)(dst_write_ptr - dst_buf_start_ptr);
  return result;
}
