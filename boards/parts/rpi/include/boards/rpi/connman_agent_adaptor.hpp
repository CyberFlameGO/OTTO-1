/*
 *  This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */
#ifndef __dbusxx__connman_agent_adaptor_hpp__ADAPTOR_MARSHALL_H
#define __dbusxx__connman_agent_adaptor_hpp__ADAPTOR_MARSHALL_H
#include <cassert>

#include <dbus-c++/dbus.h>

namespace net {
  namespace connman {

    class Agent_adaptor : public ::DBus::InterfaceAdaptor {
    public:
      Agent_adaptor() : ::DBus::InterfaceAdaptor("net.connman.Agent")
      {
        register_method(Agent_adaptor, Release, _Release_stub);
        register_method(Agent_adaptor, ReportError, _ReportError_stub);
        register_method(Agent_adaptor, RequestBrowser, _RequestBrowser_stub);
        register_method(Agent_adaptor, RequestInput, _RequestInput_stub);
      }
      const ::DBus::IntrospectedInterface* introspect() const
      {
        static const ::DBus::IntrospectedArgument Release_args[] = {{0, 0, 0}};
        static const ::DBus::IntrospectedArgument ReportError_args[] = {
          {"argin0", "o", true}, {"argin1", "s", true}, {0, 0, 0}};
        static const ::DBus::IntrospectedArgument RequestBrowser_args[] = {
          {"argin0", "o", true}, {"argin1", "s", true}, {0, 0, 0}};
        static const ::DBus::IntrospectedArgument RequestInput_args[] = {
          {"argin0", "o", true}, {"argin1", "a{sv}", true}, {"", "a{sv}", false}, {0, 0, 0}};
        static const ::DBus::IntrospectedMethod Agent_adaptor_methods[] = {{"Release", Release_args},
                                                                           {"ReportError", ReportError_args},
                                                                           {"RequestBrowser", RequestBrowser_args},
                                                                           {"RequestInput", RequestInput_args},
                                                                           {0, 0}};
        static const ::DBus::IntrospectedMethod Agent_adaptor_signals[] = {{0, 0}};
        static const ::DBus::IntrospectedProperty Agent_adaptor_properties[] = {{0, 0, 0, 0}};
        static const ::DBus::IntrospectedInterface Agent_adaptor_interface = {
          "net.connman.Agent", Agent_adaptor_methods, Agent_adaptor_signals, Agent_adaptor_properties};
        return &Agent_adaptor_interface;
      }
      /* Properties exposed by this interface.
       * Use property() and property(value) to
       * get and set a particular property.
       */
      /* Methods exported by this interface.
       * You will have to implement them in your ObjectAdaptor.
       */
      virtual void Release(::DBus::Error& error) = 0;
      virtual void ReportError(const ::DBus::Path& argin0, const std::string& argin1, ::DBus::Error& error) = 0;
      virtual void RequestBrowser(const ::DBus::Path& argin0, const std::string& argin1, ::DBus::Error& error) = 0;
      virtual std::map<std::string, ::DBus::Variant> RequestInput(const ::DBus::Path& argin0,
                                                                  const std::map<std::string, ::DBus::Variant>& argin1,
                                                                  ::DBus::Error& error) = 0;
      /* signal emitters for this interface */
    private:
      /* unmarshallers (to unpack the DBus message before calling the actual
       * interface method)
       */
      ::DBus::Message _Release_stub(const ::DBus::CallMessage& __call)
      {
        ::DBus::Error __error;
        Release(__error);
        if (__error.is_set()) return ::DBus::ErrorMessage(__call, __error.name(), __error.message());
        ::DBus::ReturnMessage __reply(__call);
        return __reply;
      }
      ::DBus::Message _ReportError_stub(const ::DBus::CallMessage& __call)
      {
        ::DBus::Error __error;
        ::DBus::MessageIter __ri = __call.reader();
        ::DBus::Path argin0;
        __ri >> argin0;
        std::string argin1;
        __ri >> argin1;
        ReportError(argin0, argin1, __error);
        if (__error.is_set()) return ::DBus::ErrorMessage(__call, __error.name(), __error.message());
        ::DBus::ReturnMessage __reply(__call);
        return __reply;
      }
      ::DBus::Message _RequestBrowser_stub(const ::DBus::CallMessage& __call)
      {
        ::DBus::Error __error;
        ::DBus::MessageIter __ri = __call.reader();
        ::DBus::Path argin0;
        __ri >> argin0;
        std::string argin1;
        __ri >> argin1;
        RequestBrowser(argin0, argin1, __error);
        if (__error.is_set()) return ::DBus::ErrorMessage(__call, __error.name(), __error.message());
        ::DBus::ReturnMessage __reply(__call);
        return __reply;
      }
      ::DBus::Message _RequestInput_stub(const ::DBus::CallMessage& __call)
      {
        ::DBus::Error __error;
        ::DBus::MessageIter __ri = __call.reader();
        ::DBus::Path argin0;
        __ri >> argin0;
        std::map<std::string, ::DBus::Variant> argin1;
        __ri >> argin1;
        std::map<std::string, ::DBus::Variant> __argout;
        __argout = RequestInput(argin0, argin1, __error);
        if (__error.is_set()) return ::DBus::ErrorMessage(__call, __error.name(), __error.message());
        ::DBus::ReturnMessage __reply(__call);
        ::DBus::MessageIter __wi = __reply.writer();
        __wi << __argout;
        return __reply;
      }
    };
  } // namespace connman
} // namespace net
#endif // __dbusxx__connman_agent_adaptor_hpp__ADAPTOR_MARSHALL_H
