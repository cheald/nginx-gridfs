/*
 * Copyright 2009 Michael Dirolf
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>

#include "client/dbclient.h"
#include "client/connpool.h"
#include "client/gridfs.h"

#include "gridfs_c_helpers.h"

extern "C" gridfile_t get_gridfile(const char* mongod_host, const char* gridfs_db, const char* gridfs_root_collection, const char* filename) {
    gridfile_t mongo_exception = {
        1,
        0,
        "",
        ""
    };
    gridfile_t no_file = {
        2,
        0,
        "",
        ""
    };
		
	std::string host = std::string(mongod_host);
	mongo::ScopedDbConnection conn(host);

    mongo::GridFS gridfs(conn.conn(), std::string(gridfs_db), std::string(gridfs_root_collection));
    mongo::GridFile gridfile = gridfs.findFile(std::string(filename));

    if (!gridfile.exists()) {
        return no_file;
    }

	std::stringstream oss (std::stringstream::in | std::stringstream::out | std::stringstream::binary);
	gridfile.write(oss);

    char* buffer;
    long size;
    size = gridfile.getContentLength();
    buffer = new char[size];
    oss.read(buffer, size);

    gridfile_t result = {
        0,
        size,
        buffer,
        gridfile.getContentType().c_str() /*"text/plain"*/ /* TODO use real mimetype */
    };

	conn.done();
    return result;
}

extern "C" void gridfile_delete(const char* data) {
	delete[] data;
}