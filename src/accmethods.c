#include <stdlib.h>

int Acc_Patternwise(int nunits, int nexamples, int iex) {
  // What we want to know is: are the activations for this example closer
  // to the intended target than the target activation associated with any
  // other esample?
  int i, jex;
  int acc = 0;
  double dist, otherdist, minotherdist, selfdist;
  double unittarg, unitact;
  minotherdist = INFINITY;
  selfdist = 0;
  for (jex=0; jex<nexamples; jex++) {
    otherdist = 0;
    for (i=0;i<nunits; i++) {
      unittarg = Root->set[0]->example[jex]->event[nevents-1].target->val[i];
      unitact = Net->group[gn]->unit[i].outputHistory[itick];
      dist = abs(unitact-unittarg);
      if (iex==jex) {
        selfdist = selfdist + dist;
      } else {
        otherdist = otherdist + dist;
      }
    }
    if (iex!=jex && otherdist < minotherdist) {
      minotherdist = otherdist;
   }
  }
  if (selfdist < minotherdist) {acc = 1;}
  return acc;
}

int Acc_Unitwise(int nunits) {
  int i;
  int acc = 0;
  double dist, otherdist, minotherdist, selfdist;
  double unittarg, unitact;
  for (i=0;i<nunits; i++) {
    unittarg = Root->set[0]->example[jex]->event[nevents-1].target->val[i];
    unitact = Net->group[gn]->unit[i].outputHistory[itick];
    dist = abs(unitact-unittarg);
  }
  if (dist < 0.5) {acc = 1;}
  return acc;
}
