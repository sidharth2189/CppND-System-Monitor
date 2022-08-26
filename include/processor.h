#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // Declare any necessary private members
 private:
 // prveious instance cpu information
 static long prevuser_, prevnice_, prevsystem_, previdle_, previowait_, \
 previrq_, prevsoftirq_, prevsteal_, prevguest_, prevguest_nice_;
 // current instance cpu information
 static long user_, nice_, system_, idle_, iowait_, irq_, softirq_, \
 steal_, guest_, guest_nice_;
};

#endif