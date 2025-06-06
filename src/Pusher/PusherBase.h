﻿/*
 * Copyright (c) 2016-present The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/ZLMediaKit/ZLMediaKit).
 *
 * Use of this source code is governed by MIT-like license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SRC_PUSHER_PUSHERBASE_H_
#define SRC_PUSHER_PUSHERBASE_H_

#include <map>
#include <memory>
#include <string>
#include <functional>
#include "Network/Socket.h"
#include "Util/mini.h"
#include "Common/MediaSource.h"

namespace mediakit {

class PusherBase : public toolkit::mINI {
public:
    using Ptr = std::shared_ptr<PusherBase>;
    using Event = std::function<void(const toolkit::SockException &ex)>;

    static Ptr createPusher(const toolkit::EventPoller::Ptr &poller,
                            const MediaSource::Ptr &src,
                            const std::string &strUrl);

    PusherBase();
    virtual ~PusherBase() = default;

    /**
     * 开始推流
     * @param strUrl 视频url，支持rtsp/rtmp
     * Start streaming
     * @param strUrl Video url, supports rtsp/rtmp
     
     * [AUTO-TRANSLATED:d1decdf6]
     */
    virtual void publish(const std::string &strUrl) {};

    /**
     * 中断推流
     * Stop streaming
     
     * [AUTO-TRANSLATED:db8d228b]
     */
    virtual void teardown() {};

    /**
     * 摄像推流结果回调
     * Camera streaming result callback
     
     * [AUTO-TRANSLATED:33825a4d]
     */
    virtual void setOnPublished(const Event &cb) = 0;

    /**
     * 设置断开回调
     * Set disconnect callback
     
     * [AUTO-TRANSLATED:b948082c]
     */
    virtual void setOnShutdown(const Event &cb) = 0;

    virtual size_t getSendSpeed() { return 0; }
    virtual size_t getSendTotalBytes() { return 0; }
	
protected:
    virtual void onShutdown(const toolkit::SockException &ex) = 0;
    virtual void onPublishResult(const toolkit::SockException &ex) = 0;
};

template<typename Parent, typename Delegate>
class PusherImp : public Parent {
public:
    using Ptr = std::shared_ptr<PusherImp>;

    template<typename ...ArgsType>
    PusherImp(ArgsType &&...args) : Parent(std::forward<ArgsType>(args)...) {}

    /**
     * 开始推流
     * @param url 推流url，支持rtsp/rtmp
     * Start streaming
     * @param url Streaming url, supports rtsp/rtmp
     
     * [AUTO-TRANSLATED:ffa95c22]
     */
    void publish(const std::string &url) override {
        return _delegate ? _delegate->publish(url) : Parent::publish(url);
    }

    /**
     * 中断推流
     * Stop streaming
     
     * [AUTO-TRANSLATED:db8d228b]
     */
    void teardown() override {
        return _delegate ? _delegate->teardown() : Parent::teardown();
    }

    std::shared_ptr<toolkit::SockInfo> getSockInfo() const {
        return std::dynamic_pointer_cast<toolkit::SockInfo>(_delegate);
    }

    /**
     * 摄像推流结果回调
     * Camera streaming result callback
     
     * [AUTO-TRANSLATED:33825a4d]
     */
    void setOnPublished(const PusherBase::Event &cb) override {
        if (_delegate) {
            _delegate->setOnPublished(cb);
        }
        _on_publish = cb;
    }

    /**
     * 设置断开回调
     * Set disconnect callback
     
     
     * [AUTO-TRANSLATED:b948082c]
     */
    void setOnShutdown(const PusherBase::Event &cb) override {
        if (_delegate) {
            _delegate->setOnShutdown(cb);
        }
        _on_shutdown = cb;
    }

    size_t getSendSpeed() override {
        return _delegate ?  _delegate->getSendSpeed() : Parent::getSendSpeed();
    }
	
   size_t getSendTotalBytes() override {
        return _delegate ? _delegate->getSendTotalBytes() : Parent::getSendTotalBytes();
    }
	
protected:
    void onShutdown(const toolkit::SockException &ex) override {
        if (_on_shutdown) {
            _on_shutdown(ex);
            _on_shutdown = nullptr;
        }
    }

    void onPublishResult(const toolkit::SockException &ex) override {
        if (_on_publish) {
            _on_publish(ex);
            _on_publish = nullptr;
        }
    }

protected:
    PusherBase::Event _on_shutdown;
    PusherBase::Event _on_publish;
    std::shared_ptr<Delegate> _delegate;
};

} /* namespace mediakit */
#endif /* SRC_PUSHER_PUSHERBASE_H_ */
