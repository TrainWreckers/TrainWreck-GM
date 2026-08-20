# **No longer under development**

Combination of things:

- Reforger quality is tanking. Errors out the wazoo. Scripting is becoming increasingly frustrating each release. Irony being Reforger was meant to be the stomping ground for modders.
- These mods were made for my friend group, and to my surprise the community enjoyed them quite a bit. Since my group has moved past Reforger, dedicating time to TrainWreck mods is hard to justify given the state of Arma.
- My fulltime job has been consuming a lot of my time. Been recovering a bit from burnout. On the side I'm trying to pursue something fun and interesting as well to help solve problems I deal with as an engineer.

Anyone is welcome to fork / pull code out that they find useful. Just be mindful things don't compile due to variable collisions.... because either a bug or a decision I don't agree with.

`this` keyword no longer works, and hasn't worked for I think a few releases now?  -- so anywhere you see me use `this` ... you'll have to find a different name. Could disambiguate by prefixing parameters with an underscore `_`, it's the simpler solution albeit against normal naming conventions for parameters.

This used to be perfectly valid Enfusion code. No longer compiles because it thinks `this.name` and `name` are the same variable. Feels like a regression but stuff like this in combination of a multitude of other errors from vanilla make me believe they aren't testing their code. 

```c
class Person
{
   string name;
   void Person(string name)
   {
       this.name = name;
   }
}
```

Documentation - https://trainwreckers.github.io/documentation
