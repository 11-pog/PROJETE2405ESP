#ifndef FixedScheduler_H
#define FixedScheduler_H

#include <Arduino.h>
#include <time.h>
#include <CppTypeDefs.h>
#include <Preferences.h>
#include <MsgPack.h>
#include <CRC32.h>
#include <algorithm>

class FixedScheduler
{
public:
    struct EventTime
    {
        byte Hours;
        byte Minutes;

        EventTime(byte hours = 12, byte minutes = 0) : Hours(hours), Minutes(minutes) {}

        bool operator==(const EventTime &other) const
        {
            return Hours == other.Hours && Minutes == other.Minutes;
        }

        bool operator!=(const EventTime &other) const
        {
            return Hours != other.Hours || Minutes != other.Minutes;
        }

        bool operator<=(const EventTime &other) const
        {
            return Hours < other.Hours || Hours == other.Hours && Minutes <= other.Minutes;
        }

        bool operator>=(const EventTime &other) const
        {
            return Hours > other.Hours || Hours == other.Hours && Minutes >= other.Minutes;
        }

        bool operator<(const EventTime &other) const
        {
            return Hours < other.Hours || Hours == other.Hours && Minutes < other.Minutes;
        }

        bool operator>(const EventTime &other) const
        {
            return Hours > other.Hours || Hours == other.Hours && Minutes > other.Minutes;
        }

        MSGPACK_DEFINE(Hours, Minutes);
    };

protected: // Structs
    struct EventData
    {
        EventTime Event;      // hours = 5 bits -> 0 - 23 AND minutes = 6 bits -> 0 - 59
        unsigned short Extra; // Full 16 bits used maybe
        unsigned int ID;

        EventData(EventTime event = EventTime(), unsigned short extra = 0);
        EventData(unsigned int ID);

        void EncodeID();

        bool operator==(const EventData &other) const
        {
            return Event == other.Event && Extra == other.Extra;
        }

        bool operator!=(const EventData &other) const
        {
            return Event != other.Event || Extra != other.Extra;
        }

        bool operator<=(const EventData &other) const
        {
            return Event <= other.Event;
        }

        bool operator>(const EventData &other) const
        {
            return Event > other.Event;
        }

        bool operator>=(const EventData &other) const
        {
            return Event >= other.Event;
        }

        bool operator<(const EventData &other) const
        {
            return Event < other.Event;
        }

        MSGPACK_DEFINE(Event, Extra, ID);
    };

    const unsigned short size;
public:
    FixedScheduler(unsigned short size);
    void begin();

    void Evaluate(DateTime now, Action<void(unsigned short)> action);
    void Schedule(EventTime ScheduledTime, unsigned short extra = 0);
    void Schedule(unsigned int ID);

    void TestPacker();
    void ResetFlash();
    void PrintScheduleList();
    void UnSchedule(unsigned int ID);
    unsigned int ReSchedule(unsigned int ID, EventTime newTime);

    using EventList = List<EventData>;

protected:
    bool IsEventDue(DateTime now);
    void SaveToFlash();
    void GetNextScheduledEvent(EventTime now);
    EventList GetDataFromFlash();
    short CheckForExistingID(unsigned int itemId);
    void SortEvents();

    
    Preferences Flash;
    EventList ScheduleList;
    EventData CurrentEvent;
    unsigned int LastExecutedEventID;
};

using EventList = typename FixedScheduler::EventList;
using EventTime = typename FixedScheduler::EventTime;

#endif