# Unreal Engine 5 Compute Shader Example

A UE 5.3 project demonstrating how to run a global compute shader through the Render Dependency Graph (RDG) and use GPU-updated render targets to animate a large instanced mesh field.

https://github.com/933yee/UE5_ComputeShader/assets/92087014/35df23f3-3bfc-4b51-90bd-0ab867989cb0

## What it demonstrates

- A runtime plugin that registers a project shader directory.
- An HLSL global compute shader compiled through Unreal's shader system.
- RDG texture registration, UAV allocation, compute-pass dispatch, and texture copy-back.
- Safe handoff of game-thread parameters to a pre-render callback.
- Texture-backed particle/instance state: velocity is read from one render target and position from another.
- GPU-driven rendering of 2,000 instances using an Instanced Static Mesh Component and a material that reads the position data.

## Data flow

~~~text
VelocityRT + PositionRT
          |
          v
FlockingComputeShader pre-render delegate
          |
          v
32 x 32-thread RDG compute dispatch
          |
          v
position = position + velocity
          |
          v
copy results back to render targets
          |
          v
instanced-material vertex placement
~~~

The example intentionally keeps the HLSL update simple. It provides the infrastructure needed for a fuller flocking/particle simulation without hiding the Unreal render-thread integration behind a third-party framework.

## Repository structure

~~~text
MyComputeShader.uproject
Plugins/FlockingComputeShader/
  Shaders/Private/ComputeShader.usf       HLSL kernel
  Source/FlockingComputeShader/           Global shader and RDG integration
Source/MyComputeShader/
  Public/Private/MyCSTestActor.*          Starts and configures dispatch
  Public/Private/InstancingRootActor.*    Creates render instances
Content/
  Scene.umap                              Ready-to-run example level
  Blueprint/                              Actor Blueprints
  Materials/                              State/init/drawing materials and RT assets
~~~

## Requirements

- Unreal Engine 5.3
- Visual Studio 2022 with the Desktop development with C++ and Game development with C++ workloads
- A GPU/RHI supporting Shader Model 5 compute shaders

## Setup

1. Clone the repository.
2. Right-click MyComputeShader.uproject and choose **Generate Visual Studio project files**.
3. Open the generated solution and build the MyComputeShaderEditor target for Development Editor.
4. Open MyComputeShader.uproject in Unreal Engine 5.3 and allow the editor to compile modules if prompted.
5. Open Content/Scene.umap and press Play.

The scene already contains Blueprint wrappers for the C++ test and instancing actors plus position/velocity render targets. The Range property is forwarded to the shader parameter structure for extending the update rule.

## Implementation details

- FlockingComputeShader loads at PostConfigInit so its virtual shader mapping exists before shader compilation.
- FComputeShaderExampleCS derives from FGlobalShader and declares its inputs through a shader parameter struct.
- Each frame, the plugin registers the external render targets with RDG, creates float RGBA UAV outputs, dispatches ceil(width/32) by ceil(height/32) thread groups, and copies the results back.
- A critical section protects cached parameters shared between the gameplay and rendering paths.

## Credits

The project was inspired by [aceyan/UE4_GPGPU_flocking](https://github.com/aceyan/UE4_GPGPU_flocking/tree/ComputeShader) and the shader-plugin patterns documented in [Temaran/UnrealEngineShaderPluginDemo](https://github.com/Temaran/UnrealEngineShaderPluginDemo).

## Notes

- The current kernel preserves velocity and performs a basic position integration step; neighborhood-based flocking forces are a natural next extension.
- Unreal projects are sensitive to engine-version changes. Opening this project in a later engine may trigger asset and build-file migration.

