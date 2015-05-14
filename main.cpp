#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <vector>

namespace pt = boost::property_tree;

struct ResetMessage
{
  std::string mUnderlyingName;
};

struct Instrument
{
private:
  std::string mUnderlyingName;
public:
  Instrument(const std::string& underlyingName)
    : mUnderlyingName(underlyingName)
  {
  }

  const std::string& GetUnderlyingName() const
  {
    return mUnderlyingName;
  }
};

// provided by trader/gui
void GenerateMessage(std::stringstream& message)
{
  pt::ptree tree;
  tree.put("resetMessage.underlyingName", "VODAFONE");
  pt::write_json(message, tree);
}

// provided by framework when it receives a message from gui/trader
pt::ptree JsonToPtree(std::stringstream& message)
{
  pt::ptree tree;
  pt::read_json(message, tree);
  return tree;
}

// provided by framework on startup (held in app config, as provided by module)
std::string GetJsonSchema(const std::string& messageName)
{
  // TODO
  return "";
}

// provided by module
std::function<bool(const Instrument&)> OnMmpReset(const pt::ptree& tree)
{
  auto underlyingName = tree.get<std::string>("resetMessage.underlyingName");
  return [underlyingName](const Instrument& instrument)
  {
    // module specific logic: underlying name is in message, and module blocks all underlyings that match
    return instrument.GetUnderlyingName() == underlyingName;
  };
}

int main()
{
  std::vector<Instrument> instruments { Instrument("VODAFONE"), Instrument("KPN") };

  // this comes from gui and is passed in
  std::stringstream message;
  GenerateMessage(message);

  // and the framework handles this (turns it into a ptree)
  auto tree = JsonToPtree(message);
  pt::write_json(std::cout, tree);

  // the ptree is passed to the module, to obtain a function that can be used to filter instruments
  auto fn = OnMmpReset(tree);

  // the framework uses the function to filter the instruments
  for (auto& instrument : instruments)
  {
    if (fn(instrument))
    {
      std::cout << "Sessions for instrument " << instrument.GetUnderlyingName() << " are now blocked" << std::endl;
    }
  }
}

