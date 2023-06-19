#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"

#include "generator.hh"
#include "run.hh"
#include "event.hh"
#include "stepping.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public:
  MyActionInitialization();
  ~MyActionInitialization();


  virtual void Build() const; // for each worker thread
  virtual void BuildForMaster() const; // for master thread
};

#endif
