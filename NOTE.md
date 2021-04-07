# Kick

- distortion https://cain-synthesizer.com/cain-cz-2
- new wavetable

- https://tonejs.github.io/docs/14.7.77/MembraneSynth.html
- https://tonejs.github.io/docs/14.7.77/Distortion
- https://stackoverflow.com/questions/22312841/waveshaper-node-in-webaudio-how-to-emulate-distortion/22313408#22313408

```js
function makeDistortionCurve( amount ) {
  var k = typeof amount === 'number' ? amount : 50,
    n_samples = 44100,
    curve = new Float32Array(n_samples),
    deg = Math.PI / 180,
    i = 0,
    x;
  for ( ; i < n_samples; ++i ) {
    x = i * 2 / n_samples - 1;
    curve[i] = ( 3 + k ) * x * 20 * deg / ( Math.PI + k * Math.abs(x) );
  }
  return curve;
};
```

```js
	set distortion(amount) {
		this._distortion = amount;
		const k = amount * 100;
		const deg = Math.PI / 180;
		this._shaper.setMap((x) => {
			if (Math.abs(x) < 0.001) {
				// should output 0 when input is 0
				return 0;
			} else {
				return (3 + k) * x * 20 * deg / (Math.PI + k * Math.abs(x));
			}
		});
	}
```
