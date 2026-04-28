# CG Assignment 08

---

##  Getting Started

1. **Clone** this repository or download it as a ZIP file.
2. Open one of the following `.sln` files in **Visual Studio 2022**:
   - `CG_Assignment08/CG_Assignment08_Q1/OpenglViewer.sln`  
   - `CG_Assignment08/CG_Assignment08_Q2/OpenglViewer.sln`

   | Q1 Project | Q2 Project |
   |------------|------------|
   | ![Q1](https://github.com/user-attachments/assets/5eebd989-e39a-4a24-be5d-6678fffe6868) | ![Q2](https://github.com/user-attachments/assets/3570fb91-8a3a-4801-bbc9-a42d8a04aaa3) |

3. Set Build Configuration
Set the configuration to:

- **Release**
- **x86**

![Setting](https://github.com/user-attachments/assets/2b01622c-27a7-41cc-8830-22e95dd69d83)

4. Press **F5** to build and run the project.  
   ![Run](https://github.com/user-attachments/assets/63ec54c4-6683-4340-8ddb-ffd54453b993)

---

##  Results

###  Q1 – Using **OpenGL Immediate Mode**
- Uses OpenGL immediate rendering  
- Lower performance  
- **FPS**: around 500  

![Q1_Result](https://github.com/user-attachments/assets/10cc519a-a7ba-4b69-a104-4272e9f0f09f)

---

###  Q2 – Using **Vertex Arrays (VAO, VBO, GPU)**
- GPU-based optimized rendering technique  
- **FPS**: around 2800  

![Q2_Result](https://github.com/user-attachments/assets/978998be-c023-4f19-9718-e15001ebea5c)

---

##  Comparison

| Method              | Performance |
|---------------------|-------------|
| OpenGL Immediate (Q1) | ~500 FPS    |
| Vertex Array (Q2)     | ~2800 FPS   |

![Comparison](https://github.com/user-attachments/assets/88041f0d-02e7-4a87-877a-0f61564e11fc)
Q2 shows **better performance** than Q1 due to its efficient GPU-based rendering with Vertex Arrays.
---

##  Notes

- This project is compatible with **Visual Studio 2022**  
- Rather than using **OpenGL**, *the rendering pipeline* was implemented manually from scratch. 
- **No additional installation is required** — just make sure you have **Visual Studio 2022** with the **C/C++ development workload** installed  
- Clone the repository and follow the steps above to get started  
- **Everything else is already included in the provided files**, so you're ready to go!

---
