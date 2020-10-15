#pragma once
#include <vector>
#include <string>

#include "../CounterHit.h"

namespace ARAPUCA{

//typedef unsigned long long int uint64_t;
typedef std::vector<uint64_t> vecUInt64;
typedef std::vector<float>    vecF;

typedef std::vector<CounterHit> CHV;

struct Counter{
    vecUInt64 PerEvent;
    vecF Observable;
    uint64_t  Sum;
    Counter() : Sum(0) { PerEvent.clear(); }
};


class Hits{
private:
    static Hits *m_Instance;
    Counter m_Counter;

    std::string m_Tag;
    Hits() : m_Counter(), m_Tag("") {}

    CHV m_CounterHits;

public:

    static Hits* Access();
    inline void Add(const uint64_t &count=1) { m_Counter.Sum += count; m_Counter.PerEvent.emplace_back(count); }
    inline uint64_t Get(const uint64_t &index) { return m_Counter.PerEvent.at(index); }
    inline uint64_t GetSum() { return m_Counter.Sum; }
    inline std::string& Tag() { return m_Tag; }
    void Clear();

    inline float GetObservable(const uint64_t &index) { return m_Counter.Observable.at(index); }
    inline vecF& GetObs() { return m_Counter.Observable; }
    inline vecF GetObs() const { return m_Counter.Observable; }
    inline void AddObservable(const float &value) { m_Counter.Observable.push_back(value); }
    inline float GetAvgObs() { 
        float avg = 0;
        if(m_Counter.Observable.size()){
            for(auto obs : m_Counter.Observable ) avg += obs;
            avg /= m_Counter.Observable.size();
        }
        return avg;
    }

    inline CHV& GetCH() { return m_CounterHits; }
    inline CHV GetCH() const { return m_CounterHits; }
    
};

}