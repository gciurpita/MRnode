## Signals Using a Processor

<img src=https://i.imgur.com/8it9OpB.jpg height=300 align=right>

The Maryland & West Virginia Railway Association was one of the reasons
we decided to move to the Cumberland, Maryland area.
The MD & WV layout has been in existence for over 15 years and
is open to the public during fairground events where it is located and
during the Christmas season.
However, the signals sat dark because like most modelers,
none of the members were familiar with more advanced electronics.
But the clubs need to sort out circuit breaker problems took priority.

The layout models the ex-B&O yard at Cumberland and
both B&O and Western Maryland mainlines are separate double track branches
that loop back on themselves.
Both mainlines have separate reversing loops under Cumberland
allowing continuous running.
The double track mainlines are broken into lengthy (~30 ft) blocks,
each protected by separate PSX circuit breakers in each direction and
DCC Specialties ARs at each end.
Sorting out the circuit breaker issues made us aware
that both the PSX circuit breakers and ARs
also have a block occupancy outputs.

<img src=https://i.imgur.com/ardpkgI.jpg height=300 align=left>

Club operation during open houses is primarily a social event
for both visitors and members.
Several trains are run on both the B&O and WM mainlines.
Not all of the layout is visible from any one spot.
With the public crowding the aisles, members tend to remain in one place
and things go well if all trains run at the same speed.
However, things don't always go well

Signals on this layout provide a useful purpose for members
to indicate trains occupying out of sight blocks or reversing loops.
And at least so far, simply need to consider the next and following blocks,
not switch settings.

I'm surprised by the reaction to the signals.
While some members were simply happy to see the signals lit up and
recognize there value,
other members reconize that they no longer need to
keep track of how far ahead the preceeding train is.
They can now run their train at varying speeds and
try to time their approach to a signal,
attempting not to stop.

The very first attempt to control a signal
used a transistor circuit built on a small 270 pine solderless breadboard.
All that circuit did was drive a set of red LEDs
mounted on top of a building.
The next attempt used a two transistor circuit to
drive either the red or green LEDs in a two masted signal.
Only the right hand signal was controlled,
the red LED in the left hand signal is permanently on.
But when it came to driving a 2nd signal,
the transistor circuit approach was immediately abandoned
to used a processor.

<img src=https://i.imgur.com/noP1Xoq.jpg width=400 align=right>
The layout has three panels (actually boxes) 
where the booster, circuit breakers and ARs reside.
This makes is difficult to indicate an approach signal
if the following block in not powered through a PSX on the same panel.
So, not all signals indicate APPROACH.

The processor code for a signal is trivial (below left)
 <table align=center width=100%> <tr>
   <td> <pre>
if (nextBlock is Occupied)
    indicate STOP;
else if (followingBlock is Occupied)
    indicate APPROACH;
else
    indicate CLEAR;
   <td> <pre>
Sig sig [] = {
 //    ID  Occ Red Yel Grn Whi  On   Nxt
    { 100,  32,  0,  2,  4,  5, HIGH },
    { 110,  33, 12, 13, 14, 15, LOW, 32  },
};
</table>

A <i>struct</i> is used to specify the pins
both for the block occupancy inputs and the signal LED outputs,
grouping all information associated with a signal
on a single line (above right).
B&O signals indicate CLEAR with a white lamp in addition to the position signal.
In addition, a field is needed
indicating that the LEDs are common cathode or anode,
whether they need to be pulled HIGH or LOW to turn on.
An <i>array</i> of structs makes it easy to support additional signals
with one additional line added to the file.

The actual code (see github)
implements the logic in two steps:
first it scans the block occupancy inputs and updates a state, and
then only if the state changes does it update the signal indication.
Scanning the blocks needs to consider if the <i>Nxt</i> block is accessible,
since its PSX may be in a different panel.

The above is only a partial implementation
of a more complete signaling system.
While unused, the code includes functions to read/write to EEPROM
as well as for using WiFi to exchange information with other <i>Nodes</i>.
WiFi hardware is needed to use WiFI and the processor used is an ESP32,
although for current code would work on an Arduino.

Code tested on the bench makes use of these capabilities.
Possible future articles can describe those changes.

Ideally the code is the same on all nodes and
differences are stored in EEPROM (or SPIFFs).
Considering that changes will be frequent while developing the system,
necessitates the need to have the ability
to easily change the configuration of a node on the layout.
This is not difficult using the serial monitor of the Arduino IDE
but requires that the code support commands to easily change
the configuration in EEPROM.

That configuration includes knowledge of the other nodes,
what blocks they monitor and signals they drive,
their name and IP addresses.
This allows nodes to forward <i>Nxt</i> block state to nodes
needing that information so they can indicate APPROACH.

Including a dispatcher PC is hoped for further down the road,
first as something that simply monitors what the layout nodes are doing and
later implementing centralized control.
Turnouts and routes are controlled with stationary decoders.
No doubt, this will take a while.

<p align=right>
Greg Ciurpita, Jan 2023
