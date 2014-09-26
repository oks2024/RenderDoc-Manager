This is a small code skeleton heavily based on the work of Temaran (https://github.com/Temaran/UE4RenderDocPlugin) that will integrate RenderDoc (https://github.com/baldurk/renderdoc) in your engine.
You just need to call the RenderDocManager constructor before the D3D initialization and you should be able to capture a frame using the StartFrameCapture() and EndFrameCapture() or by pressing the "CaptureKey" defined in the constructor.

