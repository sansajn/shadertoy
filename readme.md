# shadertoy

Shadertoy program implementuje shadertoy.com fragment shader player (pre gles2 kompatiblilné programy), takže je možné púštať shadertoy shader programy bez nutnosti pripojenia na sieť. Shadertoy naviše umožnuje pozastaviť a následne krokovať simuláciu pomocou kláves `p` (pause) a `.` (next frame).

![](assets/shadertoy_primitives_shader.png)

Ovládanie: `O` - open shader file, `R` - reload shader program, `E` - edit shader program, `P` - pause program, `.` - next frame a `H` - show help.

## spustenie

Shadertoy spustíme nasledujúcim spôsobom 

```
shadertoy [SHADER_FILE]
```

, kde `SHADER_FILE` je cesta k shadertoy.com kompatibilnému shader programu.


## kompilácia

Skompilujeme príkazom

	$ scons -j11

s adresára `shadertoy`.
