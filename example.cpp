/* An example that shows how these classes may be used.
 * 1. Declare your events. They must be derived from BaseEvent.
 * 2. Declare your event listeners. They must be derived from 
 *    EventListener<...> with one type parameter for each event
 *    class they may handle.
 * 3. Declare appropriate overloaded onEvent methods.
 *    Failure to include one makes the class abstract.
 * 4. Register your event listeners to the event dispatcher
 *    for each kind of event they handle:
 *    EventDispatcher::getInstance().addListener<EventType>(listener)
 * 5. Post events through the event dispatcher to trigger
 *    the handler functions:
 *    EventDispatcher::getInstance().post(event);
*/
#include <iostream>
#include "events.h"

using namespace std;

class AEvent : public BaseEvent {};
class BEvent : public BaseEvent {};
class DEvent : public AEvent {};  // Events can be arbitrarily derived.

class Foo : public EventListener<AEvent, DEvent, BEvent> {
public:
  void onEvent(const AEvent& evt) {
    cout << "Foo is handling AEvent" << endl;
  }

  void onEvent(const BEvent& evt) {
    cout << "Foo is handling BEvent" << endl;
  }

  // Failing to create a concrete onEvent will result in an abstract base class
  void onEvent(const DEvent& evt) {
    cout << "Foo is handling DEvent" << endl;
  }
};

class Bar : public EventListener<AEvent> {
public:
  void onEvent(const AEvent& evt) {
    cout << "Bar is handling AEvent" << endl;
  }
};

int main() {
  Foo *foo = new Foo;
  auto ed = EventDispatcher::getInstance();
  ed.addListener<AEvent>(foo);
  ed.addListener<BEvent>(foo);
  ed.addListener<DEvent>(foo);

  Bar *bar = new Bar;
  ed.addListener<AEvent>(bar);
  // A compile time error. You can't accidentally listen to unhandled events.
  // ed.addListener<BEvent>(bar);

  // A derived event can be handled by a listener for the base event.
  ed.connectHandler<DEvent, AEvent>(bar);
  // ed.connectHandler<AEvent, AEvent>(bar); // An error.
  // ed.connectHandler<DEvent, BEvent>(bar); // An error -- bar doesn't handle BEvent
  // ed.connectHandler<BEvent, AEvent>(bar); // An error -- BEvent is not derived from AEvent.

  cout << "Posting AEvent..." << endl;
  ed.post(AEvent());

  cout << "Posting BEvent..." << endl;
  ed.post(BEvent());

  cout << "Posting DEvent..." << endl;
  ed.post(DEvent());

  return 0;
}