#include <memory>
#include <type_traits>
#include <vector>

/* All events must be derived from this, to ensure that overloading
 * is handled appropriately... you'll see.
*/
class BaseEvent {};

/* Declare a templated base class that can respond to Events.
 * You choose which events your class can respond to by adding type parameters.
*/
template<class... EventTypes>
class EventListener;

/* The base case ensures that all EventListeners have a common base class.
*/
template<>
class EventListener<> {};

/* Each Event type adds an overloaded onEvent function.
 * Each Event type needs a single EventListener specialization to ensure
 * that only *one* overloaded onEvent function is defined for it.
 * It's a pure virtual function -- inheriting from EventListener defines
 * an intent, but you must supply the actual onEvent logic.
*/
template<class Event>
class EventListener<Event> : public EventListener<> {
public:
  virtual void onEvent(const Event&) = 0;
};

/* Finally the general case ensures that your EventListeners all have the
 * appropriate template specializations as base classes.
*/
template<class Event, class... Others>
class EventListener<Event, Others...> : public EventListener<Event>, public EventListener<Others...> {};

/* EventDispatcher avoids using RTTI by using templated singleton helper classes.
 * These basically amount to c++14's variable templates.
*/
class EventDispatcher {
private:
  typedef EventListener<>* EventListenerPtr;

  // Essentially a variable template
  template<class Event>
  class ListenerHelper {
  public:
    typedef std::vector<EventListenerPtr> Vec;
    typedef std::unique_ptr<Vec> Ptr;
    static Ptr listeners;
  };

public:
  static const EventDispatcher& getInstance() {
    static EventDispatcher ed;
    return ed;
  }

  template<class Event>
  void post(const Event& evt) {
    for (auto listener : (*ListenerHelper<Event>::listeners)) {
      static_cast<EventListener<Event>*>(listener)->onEvent(evt);
    }
  }

  template<class Event>
  void addListener(EventListener<Event>* el) {
    ListenerHelper<Event>::listeners->push_back(static_cast<EventListenerPtr>(el));
  }

  template<class Derived, class Base>
  void connectHandler(EventListener<Base>* el) {
    static_assert(!std::is_same<Derived, Base>::value, "Can't connect event handler to itself.");
    ListenerHelper<Derived>::listeners->push_back(static_cast<EventListenerPtr>(el));
  }

private:
  EventDispatcher() {}
};

// ZOMG
// This horrible mess is the static variable definition for ListenerHelper.
template<class Event>
typename EventDispatcher::ListenerHelper<Event>::Ptr EventDispatcher::ListenerHelper<Event>::listeners = typename EventDispatcher::ListenerHelper<Event>::Ptr(new typename EventDispatcher::ListenerHelper<Event>::Vec);
