# events

A generic event dispatcher and listener implemented in c++

# What is this?

A proof of concept inspired by event handling code like this:

```c++
class Foo : public EventListener {

  /* ... */

  void onEvent(const Event* evt) {
    switch (evt.getId()) {
      case A_EVENT:
       AEvent *aEvt = dynamic_cast<AEvent*>(evt);
       // ...
       break;
      case B_EVENT:
       BEvent *bEvt = dynamic_cast<BEvent*>(evt);
       // ...
       break;
       // etc
    }
  }
};
```

What's so bad about it?

* It uses run-time type information (`dynamic_cast`) which can introduce overhead.
* Switching on `Event::getId` virtual function is clunky and doesn't benefit from type checking.
* This style encourages a bloated onEvent function when handling many different event types.

# The solution

Consider this code:

```c++
class Foo : public EventListener<AEvent, BEvent> {

  /* ... */

  void onEvent(const AEvent& evt) {
    // ...
  }

  void onEvent(const BEvent& evt) {
    // ...
  }
};
```

It's better because:

* The implementation doesn't use run-time type information (check out `events.h`) so there's no overhead.
* It's explicit and DRY.

It's also type safe -- it will generate friendly compiler errors if you try to misuse it.

# Try it out

I built this using `g++` 5.2.0 and the following command: `g++ events.h example.cpp -std=c++11 -o example`
