# Equations

## Newton

```text
F = m a
```

## Net Force

```text
F_net = F_thrust + F_gravity + F_drag
```

## Acceleration

```text
a = F_net / m
```

For a simplified vertical reading:

```text
a = (T - D - m g) / m
```

## Gravity

```text
g(h) = (G * M) / (R + h)^2
```

## Gravity Force

```text
F_gravity = (0, -m * g)
```

## Atmospheric Density

```text
rho(h) = rho0 * exp(-h / H)
```

## Dynamic Pressure

```text
q = 0.5 * rho * v^2
```

## Drag

```text
D = 0.5 * rho * v^2 * Cd * A
```

## Drag Force

```text
F_drag = -normalize(velocity) * D
```

## Mass Loss

```text
m(t) = m0 - mdot * t
```

In the code, mass loss is clamped so fuel never becomes negative.

## Escape Velocity

```text
v_escape = sqrt(2GM / (R + h))
```

## Euler Integration

```text
state_next = state + derivative * dt
```

## RK4 Integration

```text
k1 = f(state, t)
k2 = f(state + k1*dt/2, t + dt/2)
k3 = f(state + k2*dt/2, t + dt/2)
k4 = f(state + k3*dt, t + dt)
state_next = state + dt/6 * (k1 + 2k2 + 2k3 + k4)
```
