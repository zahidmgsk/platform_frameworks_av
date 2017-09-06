/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_CLOCK_H_

#define MEDIA_CLOCK_H_

#include <map>
#include <media/stagefright/foundation/AHandler.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>

namespace android {

struct AMessage;

struct MediaClock : public AHandler {
    enum {
        TIMER_REASON_REACHED = 0,
        TIMER_REASON_NO_CLOCK = 1,
        TIMER_REASON_RESET = 2,
    };

    MediaClock();
    void init();

    void setStartingTimeMedia(int64_t startingTimeMediaUs);

    void clearAnchor();
    // It's required to use timestamp of just rendered frame as
    // anchor time in paused state.
    void updateAnchor(
            int64_t anchorTimeMediaUs,
            int64_t anchorTimeRealUs,
            int64_t maxTimeMediaUs = INT64_MAX);

    void updateMaxTimeMedia(int64_t maxTimeMediaUs);

    void setPlaybackRate(float rate);
    float getPlaybackRate() const;

    // query media time corresponding to real time |realUs|, and save the
    // result in |outMediaUs|.
    status_t getMediaTime(
            int64_t realUs,
            int64_t *outMediaUs,
            bool allowPastMaxTime = false) const;
    // query real time corresponding to media time |targetMediaUs|.
    // The result is saved in |outRealUs|.
    status_t getRealTimeFor(int64_t targetMediaUs, int64_t *outRealUs) const;

    void addTimer(const sp<AMessage> &notify, int64_t mediaTimeUs);

    void reset();

protected:
    virtual ~MediaClock();

    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    enum {
        kWhatTimeIsUp = 'tIsU',
    };

    status_t getMediaTime_l(
            int64_t realUs,
            int64_t *outMediaUs,
            bool allowPastMaxTime) const;

    void processTimers_l();

    sp<ALooper> mLooper;
    mutable Mutex mLock;

    int64_t mAnchorTimeMediaUs;
    int64_t mAnchorTimeRealUs;
    int64_t mMaxTimeMediaUs;
    int64_t mStartingTimeMediaUs;

    float mPlaybackRate;

    int32_t mGeneration;
    std::multimap<int64_t, sp<AMessage> > mTimers;

    DISALLOW_EVIL_CONSTRUCTORS(MediaClock);
};

}  // namespace android

#endif  // MEDIA_CLOCK_H_
