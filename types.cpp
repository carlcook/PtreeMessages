#include "types.h"

Instrument::Instrument(const std::string& underlyingName)
  : mUnderlyingName(underlyingName)
{
}

const std::string& Instrument::GetUnderlyingName() const
{
  return mUnderlyingName;
}

void Gui::GenerateMessage(std::stringstream& message) const
{
  pt::ptree tree;
  tree.put("resetMessage.underlyingName", "VODAFONE");
  pt::write_json(message, tree);
}

Framework::Framework()
  : mModule(new Module(*this))
{
}

pt::ptree Framework::JsonToPtree(std::stringstream& message) const
{
  pt::ptree tree;
  pt::read_json(message, tree);
  return tree;
}

std::string Framework::GetJsonSchema(MessageType) const
{
  // TODO
  return "";
}

void Framework::OnExecMmpTripped(std::function<bool(const Instrument&)> fn) const
{
  // find filtered instruments and block
  for (auto& instrument : mInstruments)
  {
    if (fn(instrument))
    {
      // we'd do this for all matching sessions for instruments (and don't forget about new sessions after the tripping)
      std::cout << "Sessions for instrument " << instrument.GetUnderlyingName() << " are now blocked" << std::endl;
    }
  }
}

void Framework::OnJsonMessage(MessageType messageType, std::stringstream& message) const
{
  // convert json message to ptree
  auto tree = JsonToPtree(message);

  switch (messageType)
  {
    case MessageType::Set:
      break;
    case MessageType::Reset:
    {
      // ask module for filter function
      auto fn = mModule->OnMmpReset(tree);

      // find filtered instruments and unblock
      for (auto& instrument : mInstruments)
      {
        if (fn(instrument))
        {
          // we'd do this for all matching sessions for instruments (and don't forget about new sessions after the tripping)
          std::cout << "Sessions for instrument " << instrument.GetUnderlyingName() << " are now unblocked" << std::endl;
        }
      }
    }
  }
}

Module::Module(const Framework& framework)
  : mFramework(framework)
{
}

const Module& Framework::GetModule() const
{
  return *mModule.get();
}

void Module::SimulateMmpBeingTripped() const
{
  // let's pretent that vodafone has been tripped
  mFramework.OnExecMmpTripped([](const Instrument& instrument)
  {
    return instrument.GetUnderlyingName() == "VODAFONE";
  });
}

std::function<bool(const Instrument&)> Module::OnMmpReset(const pt::ptree& tree) const
{
  auto underlyingName = tree.get<std::string>("resetMessage.underlyingName");
  return [underlyingName](const Instrument& instrument)
  {
    // module specific logic: underlying name is in message, and module blocks all underlyings that match
    return instrument.GetUnderlyingName() == underlyingName;
  };
}
