/**
 * WISE_Project: minizip_stream.h
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

#pragma once

#include "cwfgmp_config.h"
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <functional>


namespace hss::protobuf::io
{
	class CWFGMPROJECT_EXPORT MinizipInputStream : public google::protobuf::io::ZeroCopyInputStream
	{
	public:
		explicit MinizipInputStream(const std::string& filename, const std::function<std::string(void)>& password_callback, int buffer_size = -1);
		virtual ~MinizipInputStream();

		bool Next(const void** data, int* size) override;
		void BackUp(int count) override;
		bool Skip(int count) override;
		std::int64_t ByteCount() const override;

		inline std::int32_t GetLastError() const { return merror_; }

	protected:
		void Read();

	private:
		const std::string filename_;

		std::int32_t merror_;

		void* mcontext_;

		void* output_buffer_;
		size_t output_buffer_length_;
		size_t output_buffer_length_adjusted_;
		std::int64_t byte_count_;

		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MinizipInputStream);
	};

	class CWFGMPROJECT_EXPORT MinizipOutputStream : public google::protobuf::io::ZeroCopyOutputStream
	{
	public:
		explicit MinizipOutputStream(const std::string& filename, const std::string& password, int buffer_size = -1);
		virtual ~MinizipOutputStream();

		bool Next(void** data, int* size) override;
		void BackUp(int count) override;
		std::int64_t ByteCount() const override;
		bool Close();

	protected:
		void Write();

	private:
		const std::string password_;
		const std::string filename_;

		std::int32_t merror_;

		void* mcontext_;

		void* output_buffer_;
		size_t output_buffer_length_;
		size_t output_buffer_length_adjusted_;
		std::int64_t byte_count_;

		GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MinizipOutputStream);
	};
}
