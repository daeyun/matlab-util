///////////////////////////////////////////////////////////////////////////////
//
//  Name:        mexutil.h
//  Purpose:     Macros and helper functions for creating MATLAB MEX-files.
//  Author:      Daeyun Shin <dshin11@illinois.edu>
//  Created:     01.15.2015
//  Modified:    04.05.2015
//  Version:     0.1.2
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "mex.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <sstream>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef N_LHS_VAR
#define N_LHS_VAR nargout
#endif
#ifndef N_RHS_VAR
#define N_RHS_VAR nargin
#endif
#ifndef MEX_COMPONENT_NAME
#define MEX_COMPONENT_NAME "MATLAB"
#endif

#define BOLD(str) "<strong>" str "</strong>"
#define ORANGE(str) "[\b" str "]\b"

namespace mexutil {
enum CompOp { EQ, GT, LT, NEQ, GE, LE };

enum ArgType {
  kDouble,
  kSingle,
  kStruct,
  kLogical,
  kChar,
  kInt8,
  kUint8,
  kInt16,
  kUint16,
  kInt32,
  kUint32
};

struct StructField {
  std::string field_name;
  ArgType type;
  size_t dims[3];
  void *value;
};

struct MatlabStruct {
  std::vector<StructField> fields;
};

void createStructArray(const std::vector<MatlabStruct> &structs, mxArray *&out,
                       bool is_copy = true);

// Redirect stderr to a file or stringstream .
void CaptureErrorMsg(std::stringstream &stderr_content);
void CaptureErrorMsg(const std::string &filename);

// Constructs the identifier token used in error messages.
std::string MatlabIdStringFromFilename(std::string str);
std::string FilenameFromPath(std::string str);

// Retrieves the workspace global variable mexVerboseLevel (default: 1).
int VerboseLevel();

const int kDefaultVerboseLevel = 1;
const std::string kFilename = FilenameFromPath(__FILE__);
const std::string kFunctionIdentifier = MatlabIdStringFromFilename(kFilename);
const int kVerboseLevel = VerboseLevel();

// Force pass-by-value behavior to prevent accidentally modifying shared
// memory content in-place. Undocumented.
// http://undocumentedmatlab.com/blog/matlab-mex-in-place-editing
extern "C" bool mxUnshareArray(mxArray *array_ptr, bool noDeepCopy);

// Copy and transpose.
template <size_t nrows_in, typename T>
void Transpose(const std::vector<T> &in, T *out);

// Useful when zero-based indexing is used.
template <size_t nrows_in, typename T>
void TransposeAddOne(const std::vector<T> &in, T *out);

mxArray *UnshareArray(int index, const mxArray *prhs[]) {
  mxArray *unshared = const_cast<mxArray *>(prhs[index]);
  mxUnshareArray(unshared, true);
  return unshared;
}

bool is_invalid_id_char(char ch) { return !(isalnum((int)ch) || ch == '_'); };

std::string MatlabIdStringFromFilename(std::string str) {
  (void)(MatlabIdStringFromFilename);
  if (int i = str.find_first_of('.')) str = str.substr(0, i);
  if (!isalpha(str[0])) str = "mex_" + str;
  std::replace_if(str.begin(), str.end(), is_invalid_id_char, '_');
  return str;
}

std::string FilenameFromPath(std::string str) {
  (void)(FilenameFromPath);
  if (int i = str.find_last_of('/')) str = str.substr(i + 1, str.length());
  return str;
}

int VerboseLevel() {
  (void)(VerboseLevel);
  mxArray *ptr = mexGetVariable("global", "mexVerboseLevel");
  if (ptr == NULL) return kDefaultVerboseLevel;
  return mxGetScalar(ptr);
}

void CaptureErrorMsg(std::stringstream &stderr_content) {
  std::cerr.rdbuf(stderr_content.rdbuf());
}

#define IGNORE_RESULT(fn) if (fn) 0;
void CaptureErrorMsg(const std::string &filename) {
  IGNORE_RESULT(freopen(filename.c_str(), "a", stderr));
}

template <size_t nrows_in, typename T>
void Transpose(const std::vector<T> &in, T *out) {
  const size_t ncols_in = in.size() / nrows_in;
#pragma omp parallel for
  for (size_t i = 0; i < in.size(); i += nrows_in) {
    for (size_t j = 0; j < nrows_in; ++j) {
      *(out + (i / nrows_in) + ncols_in * j) = in[i + j];
    }
  }
}

template <size_t nrows_in, typename T>
void TransposeAddOne(const std::vector<T> &in, T *out) {
  const size_t ncols_in = in.size() / nrows_in;
#pragma omp parallel for
  for (size_t i = 0; i < in.size(); i += nrows_in) {
    for (size_t j = 0; j < nrows_in; ++j) {
      *(out + (i / nrows_in) + ncols_in * j) = in[i + j] + 1;
    }
  }
}

// e.g. LEVEL(2, MPRINTF("Not printed if logging level is less than 2."))
#define LEVEL(verbose_level, expr)            \
  {                                           \
    if (kVerboseLevel >= verbose_level) expr; \
  }

// Construct an identifier string e.g.  MATLAB:mexutil:myErrorIdentifier
#define MEX_IDENTIFIER(mnemonic)                               \
  (std::string(MEX_COMPONENT_NAME ":") + kFunctionIdentifier + \
   std::string(":" mnemonic)).c_str()

// Assert number of input variables.
#define N_IN_RANGE(min, max)                                                \
  {                                                                         \
    if (N_RHS_VAR < min || N_RHS_VAR > max) {                               \
      mexErrMsgIdAndTxt(MEX_IDENTIFIER("InputSizeError"),                   \
                        "Number of inputs must be between %d and %d.", min, \
                        max);                                               \
    }                                                                       \
  }

// Assert number of output variables.
#define N_OUT_RANGE(min, max)                                                \
  {                                                                          \
    if (N_LHS_VAR < min || N_LHS_VAR > max) {                                \
      mexErrMsgIdAndTxt(MEX_IDENTIFIER("OutputSizeError"),                   \
                        "Number of outputs must be between %d and %d.", min, \
                        max);                                                \
    }                                                                        \
  }

#define N_IN(num)                                             \
  {                                                           \
    if (N_RHS_VAR != num) {                                   \
      mexErrMsgIdAndTxt(MEX_IDENTIFIER("InputSizeError"),     \
                        "Number of inputs must be %d.", num); \
    }                                                         \
  }

#define N_OUT(num)                                             \
  {                                                            \
    if (N_LHS_VAR != num) {                                    \
      mexErrMsgIdAndTxt(MEX_IDENTIFIER("OutputSizeError"),     \
                        "Number of outputs must be %d.", num); \
    }                                                          \
  }

#define VAR(name)                                                          \
  {                                                                        \
    std::ostringstream val_str;                                            \
    val_str << name;                                                       \
    mexutil::DisplayVariable(#name, val_str.str(), sizeof(name),           \
                             (void *)&name, __FILE__, __LINE__, __func__); \
  }

#define CHECK(name)                                                           \
  {                                                                           \
    std::ostringstream val_str;                                               \
    val_str << name;                                                          \
    mexutil::DisplayVariable(#name, val_str.str(), sizeof(name), 0, __FILE__, \
                             __LINE__, __func__);                             \
  }

// Print message to MATLAB console.
// e.g.MPRINTF(BOLD("%d"), argc);
#define MPRINTF(...)           \
  {                            \
    mexPrintf(__VA_ARGS__);    \
    mexEvalString("drawnow;"); \
  }

// Display error and exit.
#define ERR_EXIT(errname, ...) \
  { mexErrMsgIdAndTxt(MEX_IDENTIFIER(errname), ##__VA_ARGS__); }

// Macros starting with an underscore are internal.
#define _M_ASSERT(condition)                                                 \
  {                                                                          \
    if (!(condition)) {                                                      \
      MPRINTF("[ERROR] (%s:%d %s) ", kFilename.c_str(), __LINE__, __func__); \
      mexErrMsgTxt("assertion " #condition " failed\n");                     \
    }                                                                        \
  }

#define _M_ASSERT_MSG(condition, msg)                                        \
  {                                                                          \
    if (!(condition)) {                                                      \
      MPRINTF("[ERROR] (%s:%d %s) ", kFilename.c_str(), __LINE__, __func__); \
      mexErrMsgIdAndTxt(MEX_IDENTIFIER("AssertionError"),                    \
                        "assertion " #condition " failed\n%s\n", msg);       \
    }                                                                        \
  }

#define _CHOOSE_MACRO(a, x, func, ...) func

#define M_ASSERT(condition, ...)                                        \
  _CHOOSE_MACRO(, ##__VA_ARGS__, _M_ASSERT_MSG(condition, __VA_ARGS__), \
                _M_ASSERT(condition))

#define M_ASSERT_FMT(condition, fmt, ...)                                    \
  {                                                                          \
    if (!(condition)) {                                                      \
      MPRINTF("[ERROR] (%s:%d %s) ", kFilename.c_str(), __LINE__, __func__); \
      mexErrMsgIdAndTxt(MEX_IDENTIFIER("AssertionError"),                    \
                        "assertion " #condition " failed\n" fmt "\n",        \
                        ##__VA_ARGS__);                                      \
    }                                                                        \
  }

}  // end of namespace mexutil
