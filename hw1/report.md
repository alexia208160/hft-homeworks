# HW1 — Order-Book Metrics

## Results
Balanced snapshot (`bs == as`) confirms `obi == 0` and `micro == mid`:
```
Balanced 2026-09-18 09:30:02.039:
mid=150.0050 spread=0.0100 micro=150.0050 obi=0.0000
```

Mid and OBI evolving across the snapshot sequence:
```
Bid heavy 2026-09-18 09:30:06.700:
mid=149.9950 spread=0.0100 micro=149.9969 obi=0.3846
  d_mid=+0.0100 d_obi=+0.3411  -> OBI rising

Bid heavy 2026-09-18 09:30:07.900:
mid=150.0050 spread=0.0100 micro=150.0082 obi=0.6471
  d_mid=+0.0100 d_obi=+0.2624  -> OBI rising

Bid heavy 2026-09-18 09:30:09.200:
mid=150.0150 spread=0.0100 micro=150.0190 obi=0.8000
  d_mid=+0.0100 d_obi=+0.1529  -> OBI rising

Ask heavy 2026-09-18 09:30:10.400:
mid=150.0150 spread=0.0100 micro=150.0116 obi=-0.6842
  d_mid=+0.0000 d_obi=-1.4842  -> OBI falling
```

## What a rising OBI suggests
A rising OBI means more size is stacked on the bid than the ask — buy-side
pressure is building, and the mid tends to move up shortly after. That's
exactly what happens in the data: OBI climbs from `+0.04` to `+0.80` and the
mid rises step by step along with it.
