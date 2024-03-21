
<h1 align="center">
 
  rt-Potts
  <br>
</h1>

<h4 align="center">Real time interactive Potts model simulation with visulization and phase transition plotting </a></h4>


<p align="center">
  <a href="#key-features">Potts model</a> •
</p>

![screenshot](https://github.com/pankmadafaki/rt-Potts/blob/main/potts.gif)

## Theoretical background

- The Potts model is a generalization of the Ising model, which represents the interaction of spins on a crystalline lattice. The simulation develops by calculating the energy
$$H_{p}= -J_{p} \sum_{(i,j)} \delta(s_{i}, s_{j})$$ with which we can calculate the energy difference $\Delta E$ between the current and previous state. The energy difference signifies whether the spin flip is energetically favourable or not. We can then use this in conjuction with the Metropolis-Hastings algorithm to decide if the spin flip shall be performed or not.
- The highest concievable energetic state is when the whole lattice has alternatig spins, i.e. would be represented in the simulation as a checkerboard pattern. The lowest energies represent a lattice filled with only one of the spins.
- Therefore the Potts model forms meta-stable regions which is similar to the formation of magnetic domains in ferromagnets and is therefore useful in the study of certain topics in solid-state physics such as phase transitions, etc.

## Code
- The model is programmed in C++ using the [Raylib](https://github.com/raysan5/raylib.git) library to facilitate visualization and realtime interaction with the model. [raylib-cpp](https://github.com/RobLoach/raylib-cpp.git) is a C++ wrapper for library and was also used in this project.

## Interactivty
- The simulation has a number of interactive inputs:
   - The **Temperature** slider, which in principle changes the probability of a flip occuring in the case of an unfavourable energy difference
   - The number of allowed $q$-states, i.e. **different spins**
   - A button which starts a **temperature sweep**, which progressively changes the temperature and measures the **mean** and **standard deviation** of the **energy** and **magnetization**
   - A button which includes also the **diagonal neighbouring elements** instead of just the usual up, down, left and right
   - A slider to introduce an external magnetic field and a slider to specify the direction of said magnetic field
 

