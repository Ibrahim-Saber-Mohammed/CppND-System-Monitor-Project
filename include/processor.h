#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 long m_prevIdle_{0};
 long m_prevNonIdle_{0};
 long m_prevTotal_{0};
};

#endif