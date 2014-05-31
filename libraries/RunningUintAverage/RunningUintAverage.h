#ifndef RunningUintAverage_h
#define RunningUintAverage_h

#define RunningUintAverage_LIB_VERSION "0.0.01"

class RunningUintAverage 
{
        public:
        RunningUintAverage(void);
        RunningUintAverage(int);
        ~RunningUintAverage();
        void clear();
        void addValue(unsigned int);
        unsigned int getAverage();
        unsigned int getMedian();

protected:
        int _size;
        int _cnt;
        int _med;
        int _idx;
        unsigned long _sum;
        unsigned int * _ar;
};

#endif
// END OF FILE
