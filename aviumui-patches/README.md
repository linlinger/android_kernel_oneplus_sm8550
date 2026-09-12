# Patches — 内核修复补丁集

本目录存放 AviumUI / OnePlus Ace 2 Pro 内核的修复补丁。

## fix-recovery-display-dangling-pointer.patch

**问题**: Recovery (TWRP) 黑屏, adb shell 有反应, 屏幕停在开机画面, TWRP 提示未就绪。

**根因**: `dsi_display_dev_probe()` (qcom/opensource/display-drivers/msm/dsi/dsi_display.c) 悬空指针 bug。
在 `#ifdef OPLUS_FEATURE_DISPLAY` 段中, `primary_display`/`secondary_display` 全局指针在
`dsi_display_validate_res()` 之前就被赋值。当 probe 失败(典型是 `-EPROBE_DEFER`, DSI 控制器
尚未就绪)走到 `end:` 时, `devm_kfree()` 释放了 `display` 内存, 但全局指针仍指向已释放的内存。
后续 secondary 面板 probe 调用 `dsi_pwr_enable_vregs()` → `get_main_display()` 读到悬空指针,
表现为 NULL → `-ENODEV` → secondary 面板初始化失败 → DRM component master 不 bind →
`/dev/dri/card0` 不注册 → TWRP 打不开显示设备 → 黑屏。

**影响**: 该 bug 存在于 OnePlusOSS 官方上游 (dsi_display.c:6583), crdroid 16.0 继承, AviumUI 继承。
正常 boot 因 probe 时序正确不触发; recovery 下因模块加载顺序不同触发。

**修复**: 在 `end:` 释放前, 将指向正在释放的 display 的全局指针清空。

### 应用方法

补丁路径相对 `kernel/oneplus/sm8550-modules/` (repo 项目根)。

```bash
cd <rom_src>/kernel/oneplus/sm8550-modules/qcom/opensource/display-drivers
git apply --check /path/to/fix-recovery-display-dangling-pointer.patch
git apply /path/to/fix-recovery-display-dangling-pointer.patch
```

或直接改源码 (dsi_display.c 的 dsi_display_dev_probe end: 标签):

```c
end:
#ifdef OPLUS_FEATURE_DISPLAY
	if (display) {
		if (primary_display == display)
			primary_display = NULL;
		if (secondary_display == display)
			secondary_display = NULL;
	}
#endif /* OPLUS_FEATURE_DISPLAY */
	if (display)
		devm_kfree(&pdev->dev, display);
```

### 验证
- 重编内核后进 recovery, 应能看到 `/dev/dri/card0`, TWRP 正常显示
- 正常 boot 显示不受影响
