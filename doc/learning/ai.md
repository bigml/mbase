## YOLOX tiny 模型为例

	WARNING: [TRT]: Using an engine plan file across different models of devices is not recommended and is likely to affect performance or even cause errors.
	0:00:12.856772075 16422   0x7f44001f30 INFO                 nvinfer gstnvinfer.cpp:651:gst_nvinfer_logger:<primary-inference> NvDsInferContext[UID 1]: Info from NvDsInferContextImpl::deserializeEngineAndBackend() <nvdsinfer_context_impl.cpp:1702> [UID = 1]: deserialized trt engine from :/home/ml/edworker/asset/yolox_4.5_tiny.engine
	INFO: [Implicit Engine Info]: layers num: 2
	0   INPUT  kFLOAT input_0         3x640x640
	1   OUTPUT kFLOAT output_0        8400x85



	WARNING: [TRT]: Using an engine plan file across different models of devices is not recommended and is likely to affect performance or even cause errors.
	0:00:12.320609830 29297   0x7f44001410 INFO                 nvinfer gstnvinfer.cpp:651:gst_nvinfer_logger:<primary-inference> NvDsInferContext[UID 1]: Info from NvDsInferContextImpl::deserializeEngineAndBackend() <nvdsinfer_context_impl.cpp:1702> [UID = 1]: deserialized trt engine from :/home/ml/edworker/asset/yoloy_n_facemask.engine
	INFO: [Implicit Engine Info]: layers num: 3
	0   INPUT  kFLOAT input_0         3x300x300
	1   OUTPUT kFLOAT scores          3000x4
	2   OUTPUT kFLOAT boxes           3000x4

### 3 x 640 x 640

    * 网络输入图片为 RGB 3通道 640 x 640 大小的图片（缩放至指定大小）

### 8400x85

    * 网络输出为 8400 块内存，每块内存内容如下所示

stride 为 8 时， 窗口锚点（anchor）共有 80 x 80 = 6400 个位置，
```
{0, 0, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{8, 0, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{16, 0, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{24, 0, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{..., 0, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{640, 0, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{0, 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{8, 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{16, 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{24, 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{..., 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{640, 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
{, 8, 8, 8, 有目标的置信度, [人的得分], [单车得分], [汽车得分], [摩托车得分] ....},
```

stride 为 16 时，窗口锚点（anchor）共有 40 x 40 = 1600 个位置，
stride 为 32 时，窗口锚点（anchor）共有 20 x 20 = 400 个位置，
