#include "types.h"

int main()
{
  Framework framework;
  Gui gui;

  // simulate the exchange tripping mmp on an active order. This will see the
  // framework asking for a function to apply as a filter for all instruments, and
  // then this filter being applied
  framework.GetModule().SimulateMmpBeingTripped();

  // simulate the gui sending in a json message
  std::stringstream resetMessage;
  gui.GenerateMessage(resetMessage);

  // The framework handles this (turns it into a ptree), passes this to the module to get
  // back a filter function, and then this filter function is applied to all instruments/sessions
  framework.OnJsonMessage(MessageType::Reset, resetMessage);
}

