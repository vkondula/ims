////////////////////////////////////////////////////////////////////////////
// Model ball.cc                SIMLIB/C++
//
// Bouncing ball (combined simulation, v.1)
//

#include "simlib.h"

const double g = 9.81;          // gravity acceleration

class Micek : ConditionDown {   // ball model description
  Integrator v,y;               // status
  unsigned count;               //
  void Action()  {              //
      Print("# %u:\n", ++count);
      Out();                    // before
      v = -0.8 * v.Value();     // loss of energy...
      y = 0;                    // numeric error elimination
      if(count>=10)             // max 10
        Stop();                 // end of simulation experiment
  }
public:
  Micek(double initialposition) :
    ConditionDown(y),           // detects change of (y>=0) from TRUE to FALSE
    v(-g),                      // y' = INTG( - m * g )
    y(v, initialposition),      // y  = INTG( y' )
    count(0) {}                 // initialize number
  void Out() {
    Print("%-9.3f  % -9f  %-9g\n",
          T.Value(), v.Value(), y.Value());
  }
};

Micek m1(1.0);                  // model

void Sample() { m1.Out(); }     // output
Sampler S(Sample,0.01);

int main() {                    // experiment description
  SetOutput("ball.dat");
  _Print("# ball - bouncing ball model\n");
  Init(0);                      // initialization of experiment
  SetStep(1e-10,0.5);           // integration step
  SetAccuracy(1e-5,0.001);      // max. integration error
  Run();                        // simulation
  return 0;
}
