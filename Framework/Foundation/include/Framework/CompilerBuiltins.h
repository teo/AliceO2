// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

// Compatibility layer for a few common builtins found in different compilers.
// These are trivial and the goal is to make sure we can compile on non GCC /
// clang compilers if requested.

#if __GNUC__
#define O2_BUILTIN_UNREACHABLE __builtin_unreachable
#elif __clang__
#define O2_BUILTIN_UNREACHABLE __builtin_unreachable
#else
#define O2_BUILTIN_UNREACHABLE
#endif
