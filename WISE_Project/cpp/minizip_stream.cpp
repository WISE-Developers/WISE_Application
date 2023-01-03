/**
 * WISE_Project: minizip_stream.cpp
 * Copyright (C) 2023  WISE
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "types.h"
#include "minizip_stream.h"
#include <zlib.h>
#include <minizip/unzip.h>
#include <minizip/zip.h>

//the zip flag definitions can be missing
#ifndef MZ_ZIP_FLAG_ENCRYPTED
#define MZ_ZIP_FLAG_ENCRYPTED           (1 << 0)
#define MZ_ZIP_FLAG_LZMA_EOS_MARKER     (1 << 1)
#define MZ_ZIP_FLAG_DEFLATE_MAX         (1 << 1)
#define MZ_ZIP_FLAG_DEFLATE_NORMAL      (0)
#define MZ_ZIP_FLAG_DEFLATE_FAST        (1 << 2)
#define MZ_ZIP_FLAG_DEFLATE_SUPER_FAST  (MZ_ZIP_FLAG_DEFLATE_FAST | \
                                         MZ_ZIP_FLAG_DEFLATE_MAX)
#define MZ_ZIP_FLAG_DATA_DESCRIPTOR     (1 << 3)
#define MZ_ZIP_FLAG_UTF8                (1 << 11)
#define MZ_ZIP_FLAG_MASK_LOCAL_INFO     (1 << 13)
#endif

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/logging.h>
#include "filesystem.hpp"


using namespace google::protobuf::io;

namespace hss::protobuf::io
{
	static const int kDefaultBufferSize = 65536;

	MinizipInputStream::MinizipInputStream(const std::string& filename, const std::function<std::string(void)>& password_callback, int buffer_size)
		: filename_(filename), merror_(0), byte_count_(0)
	{
		if (buffer_size == -1)
			output_buffer_length_ = kDefaultBufferSize;
		else
			output_buffer_length_ = buffer_size;
		output_buffer_ = operator new(output_buffer_length_);
		GOOGLE_CHECK(output_buffer_ != NULL);
		output_buffer_length_adjusted_ = output_buffer_length_;

		mcontext_ = unzOpen(filename.c_str());
		merror_ = unzGoToFirstFile(mcontext_);
		if (merror_ == UNZ_OK)
		{
			unz_file_info info;
			unzGetCurrentFileInfo(mcontext_, &info, nullptr,
				0, nullptr, 0, nullptr, 0);
			std::string password;
			if (info.flag & MZ_ZIP_FLAG_ENCRYPTED)
				password = password_callback();

			if (password.length() > 0)
				merror_ = unzOpenCurrentFilePassword(mcontext_, password.c_str());
			else
				merror_ = unzOpenCurrentFile(mcontext_);
		}
	}

	MinizipInputStream::~MinizipInputStream()
	{
		operator delete(output_buffer_);
		merror_ = unzCloseCurrentFile(mcontext_);
		merror_ = unzClose(mcontext_);
	}

	void MinizipInputStream::Read()
	{
		output_buffer_length_adjusted_ = unzReadCurrentFile(mcontext_, output_buffer_, output_buffer_length_);
		byte_count_ += output_buffer_length_adjusted_;
	}

	bool MinizipInputStream::Next(const void** data, int* size)
	{
		bool ok = (merror_ == UNZ_OK);
		if (!ok)
			return false;
		if (output_buffer_length_adjusted_ != output_buffer_length_)
			return false;

		Read();
		if (output_buffer_length_adjusted_ <= 0)
			return false;
		*data = output_buffer_;
		*size = output_buffer_length_adjusted_;
		return true;
	}

	void MinizipInputStream::BackUp(int count)
	{
		GOOGLE_CHECK_GE(output_buffer_length_adjusted_, count);
	}

	bool MinizipInputStream::Skip(int count)
	{
		return false;
	}

	std::int64_t MinizipInputStream::ByteCount() const
	{
		return byte_count_;
	}





	MinizipOutputStream::MinizipOutputStream(const std::string& filename, const std::string& password, int buffer_size)
		: password_(password), filename_(filename), merror_(0), byte_count_(0), output_buffer_length_adjusted_(0)
	{
		if (buffer_size == -1)
			output_buffer_length_ = kDefaultBufferSize;
		else
			output_buffer_length_ = buffer_size;
		output_buffer_ = operator new(output_buffer_length_);
		GOOGLE_CHECK(output_buffer_ != NULL);

		const char* pass = nullptr;
		if (password.length() > 0)
			pass = password.c_str();
		mcontext_ = zipOpen(filename.c_str(), 0);
		fs::path p(filename);
		std::string name = p.filename().replace_extension("").string();
		zip_fileinfo info;
		memset(&info, 0, sizeof(info));
		merror_ = zipOpenNewFileInZip3(mcontext_, name.c_str(),
			&info, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED,
			Z_BEST_COMPRESSION, 0, 15, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY, pass, 0);
	}

	MinizipOutputStream::~MinizipOutputStream()
	{
		operator delete(output_buffer_);
	}

	void MinizipOutputStream::Write()
	{
		merror_ = zipWriteInFileInZip(mcontext_, output_buffer_, output_buffer_length_adjusted_);
		byte_count_ += output_buffer_length_adjusted_;
		output_buffer_length_adjusted_ = 0;
	}

	bool MinizipOutputStream::Close()
	{
		bool ok = (mcontext_) && (merror_ == ZIP_OK);
		if (!ok)
			return false;
		if (output_buffer_length_adjusted_ > 0)
			Write();
		merror_ = zipCloseFileInZip(mcontext_);
		merror_ = zipClose(mcontext_, nullptr);
		return merror_ == ZIP_OK;
	}

	bool MinizipOutputStream::Next(void** data, int* size)
	{
		bool ok = (merror_ == ZIP_OK);
		if (!ok)
			return false;
		if (output_buffer_length_adjusted_ > 0)
			Write();
		*data = output_buffer_;
		*size = output_buffer_length_;
		output_buffer_length_adjusted_ = output_buffer_length_;
		return true;
	}

	void MinizipOutputStream::BackUp(int count)
	{
		GOOGLE_CHECK_GE(output_buffer_length_adjusted_, count);
		output_buffer_length_adjusted_ -= count;
	}

	std::int64_t MinizipOutputStream::ByteCount() const
	{
		return byte_count_ + output_buffer_length_adjusted_;
	}
}
