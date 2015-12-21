# ASSOA
.ASS Overlapping Analyzer

# USAGE
.\ASSOA [Numerator of Framerate] [Denominator of Framerate] [Maxium Frame] [[.ASS Filename]...]

If .ass file's end frame is equal/late than [Maxium Frame], the end frame will be set to [Maxium Frame] - 1.
If it's start frame is equal/late than [Maxium Frame] - 1, it's ignored.

# EXAMPLE
.\ASSOA 24000 1001 20000 a.ass b.css c.ass

a.ass: start: 179, end: 19999
b.ass: start: 618, end: 19999
c.ass: start: 454, end: 19999
[179 -> 453], assc: 1, assn: [a.ass]
[454 -> 617], assc: 2, assn: [a.ass, c.ass]
[618 -> 19999], assc: 3, assn: [a.ass, c.ass, b.ass]
