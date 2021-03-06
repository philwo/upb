// Copyright (c) 2009-2021, Google LLC
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Google LLC nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Google LLC BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "absl/strings/str_replace.h"
#include "upbc/common.h"

namespace upbc {
namespace {

namespace protobuf = ::google::protobuf;

void AddMessages(const protobuf::Descriptor* message,
                 std::vector<const protobuf::Descriptor*>* messages) {
  messages->push_back(message);
  for (int i = 0; i < message->nested_type_count(); i++) {
    AddMessages(message->nested_type(i), messages);
  }
}

}  // namespace

std::string StripExtension(absl::string_view fname) {
  size_t lastdot = fname.find_last_of('.');
  if (lastdot == std::string::npos) {
    return std::string(fname);
  }
  return std::string(fname.substr(0, lastdot));
}

std::string ToCIdent(absl::string_view str) {
  return absl::StrReplaceAll(str, {{".", "_"}, {"/", "_"}});
}

std::string ToPreproc(absl::string_view str) {
  return absl::AsciiStrToUpper(ToCIdent(str));
}

void EmitFileWarning(const protobuf::FileDescriptor* file, Output& output) {
  output(
      "/* This file was generated by upbc (the upb compiler) from the input\n"
      " * file:\n"
      " *\n"
      " *     $0\n"
      " *\n"
      " * Do not edit -- your changes will be discarded when the file is\n"
      " * regenerated. */\n\n",
      file->name());
}

std::vector<const protobuf::Descriptor*> SortedMessages(
    const protobuf::FileDescriptor* file) {
  std::vector<const protobuf::Descriptor*> messages;
  for (int i = 0; i < file->message_type_count(); i++) {
    AddMessages(file->message_type(i), &messages);
  }
  return messages;
}

std::string MessageName(const protobuf::Descriptor* descriptor) {
  return ToCIdent(descriptor->full_name());
}

std::string MessageInit(const protobuf::Descriptor* descriptor) {
  return MessageName(descriptor) + "_msginit";
}

}  // namespace upbc
