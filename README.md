# Go-e Charger Handsfree Auto Unlock
A Program for Wemos D1 Mini Arduino which allows you to automatically unlock your Go-E Charger when your car or phone is nearby.

How does this work?
TL;DR: The D1 Opens a wifi hotspot with a password that you save to your phone/car. Once your phone/car automatically connects when you get in range, the D1 unlocks the charger. Once you unplug your car it re-locks the charger.


Logic:
- The D1 connects to the wifi config hotspot of the Go-E charger
- the D1 locks the charger
- The D1 disconnects from the wifi
- The D1 opens a wifi hotspot
- The car/phone connect to it automatically when in range 
- The D1 kicks the car/phone from the hotspot and closes it
- The D1 connects to the wifi config hotspot of the Go-E charger
- The D1 unlocks the charger
- The D1 stays connected to the charger and waits until the car is unplugged and 5 minutes pass. (The 5 Minutes do not expire while a car is plugged in)
- return to step 2


The charger is initially locked so you can have it in a default state of unlocked, to be easily able to take it with you without needing to reconfigure it via the app. 

Limitations:
- you need to save this hotspot to the car or phone once, and since the D1 does not provide internet, you may have to open a hotspot from your phone with similar credentials
- this only works 100% if there are not other Hotspots that your car or phone will connect to nearby. otherwise the car or phone may be stuck in the other access point until it tries to switch. If it does not switch, try setting them to the same name and pw.
- You need to plug in the Wemos D1 Mini near the Go-E charger, and also in wifi range of the car
- If someone unplugs the go-e charger they can reset it to unlocked if that is your default state, or they can just plug in their own charger. Prevent people from unplugging it.


Note: The code is kind of jank, but it works. I never intended this to adhere to any standards, Just to take as little time as possible.
