
#include <util/atomic.h>

// Which pin is ShakeFinder's output connected to? (It
// should be one of the pins with an "INTx" interrupt.)
static constexpr int ShakeFinderPin = 3;

// This ISR will be called whenever ShakeFinder's output
// goes low (which means it detected vibration).
volatile bool shakeDetected = false;
static void ShakeISR() noexcept { shakeDetected = true; }

// Call this anytime you'd like to know whether shaking was
// detected since the last time you called this.  It acts as
// both the way to retrieve the value and the way to reset it.
bool ShookOrShakingWithReset()
{
  bool shook = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    shook = shakeDetected;
    shakeDetected = false;
  }

  const bool stillShaking = digitalRead(ShakeFinderPin) == 0;
  return shook || stillShaking;
}

void setup()
{
  // INPUT_PULLUP enables the internal 50k pull-up that
  // ShakeFinder needs for its open-drain output.
  pinMode(ShakeFinderPin, INPUT_PULLUP);

  // ShakeFinder pulls its output pin low whenever
  // shaking is detected, so we look for FALLING edges.
  attachInterrupt(digitalPinToInterrupt(ShakeFinderPin), ShakeISR, FALLING);

  // Just for this demo.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // For now, let's just echo ShakeFinder's state by reading
  // its output directly.
  digitalWrite(LED_BUILTIN, !digitalRead(ShakeFinderPin));

  // You can do work here and call ShookOrShakingWithReset as
  // infrequently as you like.  The general pattern when you
  // need to do a critical task will look something like this:

  //
  // do
  // {
  //   // Wait until things aren't moving.
  //   while (ShookOrShakingWithReset()) { }
  //
  //   // TODO: Do your important, long-running task here.
  //
  // } while (!ShookOrShakingWithReset());
  //

  // The first, empty while() loop busy-waits until shaking
  // isn't detected.  The do-while loop will keep repeating
  // your task until it was completed without any shaking.
}
