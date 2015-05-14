#ifndef TYPES_H
#define TYPES_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <vector>

namespace pt = boost::property_tree;

/// enums
enum class MessageType
{
  Set,
  Reset
};

/// messages
struct ResetMessage
{
  std::string mUnderlyingName;
};

/// forward declarations
class Module;

/// class definitions
class Instrument
{
private:
  std::string mUnderlyingName;
public:
  Instrument(const std::string& underlyingName);
  const std::string& GetUnderlyingName() const;
};

class Gui
{
public:
  void GenerateMessage(std::stringstream& message) const;
};

class Framework
{
private:
  /// members
  const std::unique_ptr<Module> mModule;
  std::vector<Instrument> mInstruments { Instrument("VODAFONE"), Instrument("KPN") };

  /// useful methods
  pt::ptree JsonToPtree(std::stringstream& message) const;
  std::string GetJsonSchema(MessageType) const;
public:
  Framework();
  const Module& GetModule() const;
  void OnExecMmpTripped(std::function<bool(const Instrument&)> fn) const;
  void OnJsonMessage(MessageType, std::stringstream&) const;
};

class Module
{
private:
  const Framework& mFramework;
public:
  Module(const Framework&);
  void SimulateMmpBeingTripped() const;
  std::function<bool(const Instrument&)> OnMmpReset(const pt::ptree& tree) const;
};

#endif // TYPES_H
