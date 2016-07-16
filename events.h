#include <map>
#include <typeindex>
#include <typeinfo>
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

/* Note that the typeid uses below can be resolved at compile-time.
 * We make no use of run-time type information (RTTI).
*/
class EventDispatcher {
public:
  static const EventDispatcher& getInstance() {
    static EventDispatcher ed;
    return ed;
  }

  template<class Event>
  void post(const Event& evt) {
    for (auto listener : listeners[std::type_index(typeid(Event))]) {
      static_cast<EventListener<Event>*>(listener)->onEvent(evt);
    }
  }

  template<class Event>
  void addListener(EventListener<Event>* el) {
    listeners[std::type_index(typeid(Event))].push_back(el);
  }

  template<class Derived, class Base>
  void connectHandler(EventListener<Base>* el) {
    static_assert(!std::is_same<Derived, Base>::value, "Can't connect event handler to itself.");
    listeners[std::type_index(typeid(Derived))].push_back(el);
  }

private:
  EventDispatcher() {};

  std::map<std::type_index, std::vector<EventListener<>*>> listeners;
};