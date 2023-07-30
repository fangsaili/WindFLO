//
//  main.cpp
//

#include <iostream>
#include <stdio.h>

#include "GA.h"
#include "KusiakLayoutEvaluator.h"

int main(int argc, const char * argv[]) {
  for (int i=1; i < argc; i++) {
    cout << "========= Starting: " << argv[i] << " ========= " << endl;
    WindScenario wsc(argv[i]);
    KusiakLayoutEvaluator wfle;
    wfle.initialize(wsc);
    GA ga(wfle, i-1);
    ga.run();
  }
}
