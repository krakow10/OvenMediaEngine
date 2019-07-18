//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Jaejong Bong
//  Copyright (c) 2019 AirenSoft. All rights reserved.
//
//==============================================================================

#pragma once

#include "segment_stream/segment_stream_server.h"
#include "cmaf_interceptor.h"
#include "cmaf_packetyzer.h"

struct CmafHttpChunkedData
{
public:

	CmafHttpChunkedData( )
	{
		chunked_data = std::make_shared<ov::Data>(104*1024);
	}

	void AddChunkData(const std::shared_ptr<ov::Data> &data)
	{
		chunked_data->Append(data->GetData(), data->GetLength());
	}

	std::shared_ptr<ov::Data> chunked_data;
	std::vector<std::shared_ptr<HttpResponse>> response_list;
};

//====================================================================================================
// CmafStreamServer
//====================================================================================================
class CmafStreamServer : public SegmentStreamServer, public ICmafChunkedTransfer
{
public :
    CmafStreamServer() = default;

    ~CmafStreamServer() = default;

public :
    cfg::PublisherType GetPublisherType() override
    {
        return cfg::PublisherType::Cmaf;
    }

    std::shared_ptr<SegmentStreamInterceptor> CreateInterceptor() override
    {
       auto interceptor = std::make_shared<CmafInterceptor>();
       return std::static_pointer_cast<SegmentStreamInterceptor>(interceptor);
    }

protected:

	// Implement SegmentStreamServer
    void ProcessRequestStream(const std::shared_ptr<HttpResponse> &response,
							   const ov::String &app_name,
							   const ov::String &stream_name,
							   const ov::String &file_name,
							   const ov::String &file_ext) override;

	void PlayListRequest(const ov::String &app_name,
						 const ov::String &stream_name,
						 const ov::String &file_name,
						 PlayListType play_list_type,
						 const std::shared_ptr<HttpResponse> &response) override;

	void SegmentRequest(const ov::String &app_name,
						const ov::String &stream_name,
						const ov::String &file_name,
						SegmentType segment_type,
						const std::shared_ptr<HttpResponse> &response) override;


	// Implement ICmafChunkedTransfer
	void OnCmafChunkDataPush(const ov::String &app_name,
							 const ov::String &stream_name,
							 const ov::String &file_name,
							 bool is_video,
							 std::shared_ptr<ov::Data> &chunk_data) override;

	void OnCmafChunkedComplete(const ov::String &app_name,
							   const ov::String &stream_name,
							   const ov::String &file_name,
							   bool is_video) override;


private:

	// key : (app name)/(stream name)/ (file name)
	std::map<ov::String, std::shared_ptr<CmafHttpChunkedData>> _http_chunked_video_list;
	std::mutex _htttp_chunked_video_guard;

	std::map<ov::String, std::shared_ptr<CmafHttpChunkedData>> _http_chunked_audio_list;
	std::mutex _htttp_chunked_audio_guard;

};


