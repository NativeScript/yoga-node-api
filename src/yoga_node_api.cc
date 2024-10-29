#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi_util.h"
#include "yoga/YGConfig.h"
#include "yoga/YGNode.h"
#include "yoga/YGNodeLayout.h"
#include "yoga/YGNodeStyle.h"
#include <cstddef>
#include <cstdio>

// Objects

inline napi_value YGValueToJS(napi_env env, YGValue const &value) {
  napi_value obj;
  napi_create_object(env, &obj);
  napi_set_named_property(env, obj, "unit", js_int32(env, value.unit));
  napi_set_named_property(env, obj, "value", js_double(env, value.value));
  return obj;
}

// class Config {

NAPI_FUNCTION(Config_constructor) {
  napi_value jsThis;
  napi_get_cb_info(env, cbinfo, NULL, NULL, &jsThis, NULL);
  YGConfigRef config = YGConfigNew();
  napi_wrap(env, jsThis, config, NULL, NULL, NULL);
  return jsThis;
}

NAPI_FUNCTION(Config_create) {
  napi_value jsThis;
  napi_get_cb_info(env, cbinfo, NULL, NULL, &jsThis, NULL);
  napi_value instance;
  napi_new_instance(env, jsThis, 0, NULL, &instance);
  return instance;
}

NAPI_FUNCTION(Config_destroy) {
  napi_value arg;
  size_t argc = 1;
  napi_get_cb_info(env, cbinfo, &argc, &arg, NULL, NULL);
  YGConfigRef config = (YGConfigRef)unwrap(env, arg);
  YGConfigFree(config);
  return NULL;
}

NAPI_FUNCTION(Config_free) {
  NAPI_METHOD_HEADER_NO_ARGS(YGConfigRef, config);
  YGConfigFree(config);
  return NULL;
}

NAPI_FUNCTION(Config_setExperimentalFeatureEnabled) {
  NAPI_METHOD_HEADER(YGConfigRef, config, 2);
  NAPI_ARG_INT32(feature, 0);
  NAPI_ARG_BOOL(enabled, 1);
  YGConfigSetExperimentalFeatureEnabled(
      config, static_cast<YGExperimentalFeature>(feature), enabled);
  return NULL;
}

NAPI_FUNCTION(Config_setPointScaleFactor) {
  NAPI_METHOD_HEADER(YGConfigRef, config, 1);
  NAPI_ARG_DOUBLE(pixelsInPoint, 0);
  YGConfigSetPointScaleFactor(config, pixelsInPoint);
  return NULL;
}

NAPI_FUNCTION(Config_setErrata) {
  NAPI_METHOD_HEADER(YGConfigRef, config, 1);
  NAPI_ARG_INT32(errata, 0);
  YGConfigSetErrata(config, static_cast<YGErrata>(errata));
  return NULL;
}

NAPI_FUNCTION(Config_setUseWebDefaults) {
  NAPI_METHOD_HEADER(YGConfigRef, config, 1);
  NAPI_ARG_BOOL(useWebDefaults, 0);
  YGConfigSetUseWebDefaults(config, useWebDefaults);
  return NULL;
}

NAPI_FUNCTION(Config_isExperimentalFeatureEnabled) {
  NAPI_METHOD_HEADER(YGConfigRef, config, 1);
  NAPI_ARG_INT32(feature, 0);
  bool enabled = YGConfigIsExperimentalFeatureEnabled(
      config, static_cast<YGExperimentalFeature>(feature));
  return js_bool(env, enabled);
}

NAPI_FUNCTION(Config_getErrata) {
  NAPI_METHOD_HEADER_NO_ARGS(YGConfigRef, config);
  int errata = static_cast<int>(YGConfigGetErrata(config));
  return js_int32(env, errata);
}

NAPI_FUNCTION(Config_useWebDefaults) {
  NAPI_METHOD_HEADER_NO_ARGS(YGConfigRef, config);
  bool useWebDefaults = YGConfigGetUseWebDefaults(config);
  return js_bool(env, useWebDefaults);
}

// } /* class Config */

// class Node {

NAPI_FUNCTION(Node_constructor) {
  napi_value jsThis;
  size_t argc = 1;
  napi_value config;
  napi_get_cb_info(env, cbinfo, &argc, &config, &jsThis, NULL);
  YGNodeRef node = argc == 1
                       ? YGNodeNewWithConfig((YGConfigRef)unwrap(env, config))
                       : YGNodeNew();
  napi_wrap(env, jsThis, node, NULL, NULL, NULL);
  napi_ref ref;
  napi_create_reference(env, jsThis, 1, &ref);
  YGNodeSetContext(node, (void *)ref);
  return jsThis;
}

NAPI_FUNCTION(Node_createDefault) {
  napi_value jsThis;
  napi_get_cb_info(env, cbinfo, NULL, NULL, &jsThis, NULL);
  napi_value instance;
  napi_new_instance(env, jsThis, 0, NULL, &instance);
  return instance;
}

NAPI_FUNCTION(Node_createWithConfig) {
  napi_value jsThis;
  napi_value arg;
  size_t argc = 1;
  napi_get_cb_info(env, cbinfo, &argc, &arg, &jsThis, NULL);
  napi_value result;
  napi_new_instance(env, jsThis, argc, &arg, &result);
  return result;
}

NAPI_FUNCTION(Node_destroy) {
  napi_value arg;
  size_t argc = 1;
  napi_get_cb_info(env, cbinfo, &argc, &arg, NULL, NULL);
  YGNodeRef node = (YGNodeRef)unwrap(env, arg);
  napi_ref ref = (napi_ref)YGNodeGetContext(node);
  napi_delete_reference(env, ref);
  YGNodeFree(node);
  return NULL;
}

NAPI_FUNCTION(Node_free) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  napi_ref ref = (napi_ref)YGNodeGetContext(node);
  napi_delete_reference(env, ref);
  YGNodeFree(node);
  return NULL;
}

void freeNodeRecursive(napi_env env, YGNodeRef node) {
  for (unsigned t = 0, T = YGNodeGetChildCount(node); t < T; t++) {
    freeNodeRecursive(env, YGNodeGetChild(node, 0));
  }
  napi_ref ref = (napi_ref)YGNodeGetContext(node);
  napi_delete_reference(env, ref);
  YGNodeFree(node);
}

NAPI_FUNCTION(Node_freeRecursive) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  freeNodeRecursive(env, node);
  return NULL;
}

NAPI_FUNCTION(Node_reset) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGNodeReset(node);
  return NULL;
}

NAPI_FUNCTION(Node_copyStyle) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  YGNodeCopyStyle(node, (YGNodeRef)unwrap(env, argv[0]));
  return NULL;
}

NAPI_FUNCTION(Node_setPositionType) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(positionType, 0);
  YGNodeStyleSetPositionType(node, static_cast<YGPositionType>(positionType));
  return NULL;
}

NAPI_FUNCTION(Node_setPosition) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(position, 1);
  YGNodeStyleSetPosition(node, static_cast<YGEdge>(edge), position);
  return NULL;
}

NAPI_FUNCTION(Node_setPositionPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(position, 1);
  YGNodeStyleSetPositionPercent(node, static_cast<YGEdge>(edge), position);
  return NULL;
}

NAPI_FUNCTION(Node_setAlignContent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(alignContent, 0);
  YGNodeStyleSetAlignContent(node, static_cast<YGAlign>(alignContent));
  return NULL;
}

NAPI_FUNCTION(Node_setAlignItems) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(alignItems, 0);
  YGNodeStyleSetAlignItems(node, static_cast<YGAlign>(alignItems));
  return NULL;
}

NAPI_FUNCTION(Node_setAlignSelf) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(alignSelf, 0);
  YGNodeStyleSetAlignSelf(node, static_cast<YGAlign>(alignSelf));
  return NULL;
}

NAPI_FUNCTION(Node_setFlexDirection) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(flexDirection, 0);
  YGNodeStyleSetFlexDirection(node,
                              static_cast<YGFlexDirection>(flexDirection));
  return NULL;
}

NAPI_FUNCTION(Node_setFlexWrap) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(flexWrap, 0);
  YGNodeStyleSetFlexWrap(node, static_cast<YGWrap>(flexWrap));
  return NULL;
}

NAPI_FUNCTION(Node_setJustifyContent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(justifyContent, 0);
  YGNodeStyleSetJustifyContent(node, static_cast<YGJustify>(justifyContent));
  return NULL;
}

NAPI_FUNCTION(Node_setMargin) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(margin, 1);
  YGNodeStyleSetMargin(node, static_cast<YGEdge>(edge), margin);
  return NULL;
}

NAPI_FUNCTION(Node_setMarginPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(margin, 1);
  YGNodeStyleSetMarginPercent(node, static_cast<YGEdge>(edge), margin);
  return NULL;
}

NAPI_FUNCTION(Node_setMarginAuto) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  YGNodeStyleSetMarginAuto(node, static_cast<YGEdge>(edge));
  return NULL;
}

NAPI_FUNCTION(Node_setOverflow) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(overflow, 0);
  YGNodeStyleSetOverflow(node, static_cast<YGOverflow>(overflow));
  return NULL;
}

NAPI_FUNCTION(Node_setDisplay) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(display, 0);
  YGNodeStyleSetDisplay(node, static_cast<YGDisplay>(display));
  return NULL;
}

NAPI_FUNCTION(Node_setFlex) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(flex, 0);
  YGNodeStyleSetFlex(node, flex);
  return NULL;
}

NAPI_FUNCTION(Node_setFlexBasis) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(flexBasis, 0);
  YGNodeStyleSetFlexBasis(node, flexBasis);
  return NULL;
}

NAPI_FUNCTION(Node_setFlexBasisPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(flexBasis, 0);
  YGNodeStyleSetFlexBasisPercent(node, flexBasis);
  return NULL;
}

NAPI_FUNCTION(Node_setFlexBasisAuto) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 0);
  YGNodeStyleSetFlexBasisAuto(node);
  return NULL;
}

NAPI_FUNCTION(Node_setFlexGrow) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(flexGrow, 0);
  YGNodeStyleSetFlexGrow(node, flexGrow);
  return NULL;
}

NAPI_FUNCTION(Node_setFlexShrink) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(flexShrink, 0);
  YGNodeStyleSetFlexShrink(node, flexShrink);
  return NULL;
}

NAPI_FUNCTION(Node_setWidth) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(width, 0);
  YGNodeStyleSetWidth(node, width);
  return NULL;
}

NAPI_FUNCTION(Node_setWidthPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(width, 0);
  YGNodeStyleSetWidthPercent(node, width);
  return NULL;
}

NAPI_FUNCTION(Node_setWidthAuto) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 0);
  YGNodeStyleSetWidthAuto(node);
  return NULL;
}

NAPI_FUNCTION(Node_setHeight) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(height, 0);
  YGNodeStyleSetHeight(node, height);
  return NULL;
}

NAPI_FUNCTION(Node_setHeightPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(height, 0);
  YGNodeStyleSetHeightPercent(node, height);
  return NULL;
}

NAPI_FUNCTION(Node_setHeightAuto) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 0);
  YGNodeStyleSetHeightAuto(node);
  return NULL;
}

NAPI_FUNCTION(Node_setMinWidth) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(minWidth, 0);
  YGNodeStyleSetMinWidth(node, minWidth);
  return NULL;
}

NAPI_FUNCTION(Node_setMinWidthPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(minWidth, 0);
  YGNodeStyleSetMinWidthPercent(node, minWidth);
  return NULL;
}

NAPI_FUNCTION(Node_setMinHeight) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(minHeight, 0);
  YGNodeStyleSetMinHeight(node, minHeight);
  return NULL;
}

NAPI_FUNCTION(Node_setMinHeightPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(minHeight, 0);
  YGNodeStyleSetMinHeightPercent(node, minHeight);
  return NULL;
}

NAPI_FUNCTION(Node_setMaxWidth) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(maxWidth, 0);
  YGNodeStyleSetMaxWidth(node, maxWidth);
  return NULL;
}

NAPI_FUNCTION(Node_setMaxWidthPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(maxWidth, 0);
  YGNodeStyleSetMaxWidthPercent(node, maxWidth);
  return NULL;
}

NAPI_FUNCTION(Node_setMaxHeight) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(maxHeight, 0);
  YGNodeStyleSetMaxHeight(node, maxHeight);
  return NULL;
}

NAPI_FUNCTION(Node_setMaxHeightPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(maxHeight, 0);
  YGNodeStyleSetMaxHeightPercent(node, maxHeight);
  return NULL;
}

NAPI_FUNCTION(Node_setAspectRatio) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_DOUBLE(aspectRatio, 0);
  YGNodeStyleSetAspectRatio(node, aspectRatio);
  return NULL;
}

NAPI_FUNCTION(Node_setBorder) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(border, 1);
  YGNodeStyleSetBorder(node, static_cast<YGEdge>(edge), border);
  return NULL;
}

NAPI_FUNCTION(Node_setPadding) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(padding, 1);
  YGNodeStyleSetPadding(node, static_cast<YGEdge>(edge), padding);
  return NULL;
}

NAPI_FUNCTION(Node_setPaddingPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(edge, 0);
  NAPI_ARG_DOUBLE(padding, 1);
  YGNodeStyleSetPaddingPercent(node, static_cast<YGEdge>(edge), padding);
  return NULL;
}

NAPI_FUNCTION(Node_setGap) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(gutter, 0);
  NAPI_ARG_DOUBLE(gapLength, 1);
  YGNodeStyleSetGap(node, static_cast<YGGutter>(gutter), gapLength);
  return NULL;
}

NAPI_FUNCTION(Node_setGapPercent) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  NAPI_ARG_INT32(gutter, 0);
  NAPI_ARG_DOUBLE(gapLength, 1);
  YGNodeStyleSetGapPercent(node, static_cast<YGGutter>(gutter), gapLength);
  return NULL;
}

NAPI_FUNCTION(Node_setDirection) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(direction, 0);
  YGNodeStyleSetDirection(node, static_cast<YGDirection>(direction));
  return NULL;
}

NAPI_FUNCTION(Node_getPositionType) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int positionType = YGNodeStyleGetPositionType(node);
  return js_int32(env, positionType);
}

NAPI_FUNCTION(Node_getPosition) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  YGValue position = YGNodeStyleGetPosition(node, static_cast<YGEdge>(edge));
  return YGValueToJS(env, position);
}

NAPI_FUNCTION(Node_getAlignContent) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int alignContent = YGNodeStyleGetAlignContent(node);
  return js_int32(env, alignContent);
}

NAPI_FUNCTION(Node_getAlignItems) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int alignItems = YGNodeStyleGetAlignItems(node);
  return js_int32(env, alignItems);
}

NAPI_FUNCTION(Node_getAlignSelf) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int alignSelf = YGNodeStyleGetAlignSelf(node);
  return js_int32(env, alignSelf);
}

NAPI_FUNCTION(Node_getFlexDirection) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int flexDirection = YGNodeStyleGetFlexDirection(node);
  return js_int32(env, flexDirection);
}

NAPI_FUNCTION(Node_getFlexWrap) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int flexWrap = YGNodeStyleGetFlexWrap(node);
  return js_int32(env, flexWrap);
}

NAPI_FUNCTION(Node_getJustifyContent) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int justifyContent = YGNodeStyleGetJustifyContent(node);
  return js_int32(env, justifyContent);
}

NAPI_FUNCTION(Node_getMargin) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  YGValue margin = YGNodeStyleGetMargin(node, static_cast<YGEdge>(edge));
  return YGValueToJS(env, margin);
}

NAPI_FUNCTION(Node_getFlexBasis) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue flexBasis = YGNodeStyleGetFlexBasis(node);
  return YGValueToJS(env, flexBasis);
}

NAPI_FUNCTION(Node_getFlexGrow) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  double flexGrow = YGNodeStyleGetFlexGrow(node);
  return js_double(env, flexGrow);
}

NAPI_FUNCTION(Node_getFlexShrink) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  double flexShrink = YGNodeStyleGetFlexShrink(node);
  return js_double(env, flexShrink);
}

NAPI_FUNCTION(Node_getWidth) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue width = YGNodeStyleGetWidth(node);
  return YGValueToJS(env, width);
}

NAPI_FUNCTION(Node_getHeight) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue height = YGNodeStyleGetHeight(node);
  return YGValueToJS(env, height);
}

NAPI_FUNCTION(Node_getMinWidth) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue minWidth = YGNodeStyleGetMinWidth(node);
  return YGValueToJS(env, minWidth);
}

NAPI_FUNCTION(Node_getMinHeight) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue minHeight = YGNodeStyleGetMinHeight(node);
  return YGValueToJS(env, minHeight);
}

NAPI_FUNCTION(Node_getMaxWidth) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue maxWidth = YGNodeStyleGetMaxWidth(node);
  return YGValueToJS(env, maxWidth);
}

NAPI_FUNCTION(Node_getMaxHeight) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGValue maxHeight = YGNodeStyleGetMaxHeight(node);
  return YGValueToJS(env, maxHeight);
}

NAPI_FUNCTION(Node_getAspectRatio) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  double aspectRatio = YGNodeStyleGetAspectRatio(node);
  return js_double(env, aspectRatio);
}

NAPI_FUNCTION(Node_getBorder) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  float border = YGNodeStyleGetBorder(node, static_cast<YGEdge>(edge));
  return js_double(env, border);
}

NAPI_FUNCTION(Node_getOverflow) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int overflow = YGNodeStyleGetOverflow(node);
  return js_int32(env, overflow);
}

NAPI_FUNCTION(Node_getDisplay) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int display = YGNodeStyleGetDisplay(node);
  return js_int32(env, display);
}

NAPI_FUNCTION(Node_getPadding) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  YGValue padding = YGNodeStyleGetPadding(node, static_cast<YGEdge>(edge));
  return YGValueToJS(env, padding);
}

NAPI_FUNCTION(Node_getGap) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(gutter, 0);
  float gap = YGNodeStyleGetGap(node, static_cast<YGGutter>(gutter));
  return js_double(env, gap);
}

NAPI_FUNCTION(Node_insertChild) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 2);
  YGNodeRef child = (YGNodeRef)unwrap(env, argv[0]);
  NAPI_ARG_INT32(index, 1);
  YGNodeInsertChild(node, child, index);
  return NULL;
}

NAPI_FUNCTION(Node_removeChild) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  YGNodeRef child = (YGNodeRef)unwrap(env, argv[0]);
  YGNodeRemoveChild(node, child);
  return NULL;
}

NAPI_FUNCTION(Node_getChildCount) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  unsigned count = YGNodeGetChildCount(node);
  return js_int32(env, count);
}

NAPI_FUNCTION(Node_getParent) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGNodeRef parent = YGNodeGetParent(node);
  if (parent == NULL) {
    return NULL;
  }
  napi_ref ref = (napi_ref)YGNodeGetContext(parent);
  if (ref == NULL) {
    return NULL;
  }
  napi_value jsParent = NULL;
  napi_get_reference_value(env, ref, &jsParent);
  return jsParent;
}

NAPI_FUNCTION(Node_getChild) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(index, 0);
  YGNodeRef child = YGNodeGetChild(node, index);
  if (child == NULL) {
    return NULL;
  }
  napi_ref ref = (napi_ref)YGNodeGetContext(child);
  if (ref == NULL) {
    return NULL;
  }
  napi_value jsChild = NULL;
  napi_get_reference_value(env, ref, &jsChild);
  return jsChild;
}

NAPI_FUNCTION(Node_setAlwaysFormsContainingBlock) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_BOOL(always, 0);
  YGNodeSetAlwaysFormsContainingBlock(node, always);
  return NULL;
}

NAPI_FUNCTION(Node_isReferenceBaseline) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  bool isReferenceBaseline = YGNodeIsReferenceBaseline(node);
  return js_bool(env, isReferenceBaseline);
}

NAPI_FUNCTION(Node_setIsReferenceBaseline) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_BOOL(isReferenceBaseline, 0);
  YGNodeSetIsReferenceBaseline(node, isReferenceBaseline);
  return NULL;
}

thread_local napi_env global_env;

static YGSize globalMeasureFunc(YGNodeConstRef nodeRef, float width,
                                YGMeasureMode widthMode, float height,
                                YGMeasureMode heightMode) {
  napi_ref ref = (napi_ref)(YGNodeGetContext(nodeRef));

  napi_value jsThis;
  napi_get_reference_value(global_env, ref, &jsThis);

  napi_value measureFunc;
  napi_get_named_property(global_env, jsThis, "_measureFunc", &measureFunc);

  napi_value argv[4];
  argv[0] = js_double(global_env, width);
  argv[1] = js_int32(global_env, widthMode);
  argv[2] = js_double(global_env, height);
  argv[3] = js_int32(global_env, heightMode);

  napi_value result;
  napi_call_function(global_env, jsThis, measureFunc, 4, argv, &result);

  napi_value widthValue, heightValue;
  napi_get_named_property(global_env, result, "width", &widthValue);
  napi_get_named_property(global_env, result, "height", &heightValue);

  double dwidth, dheight;
  napi_get_value_double(global_env, widthValue, &dwidth);
  napi_get_value_double(global_env, heightValue, &dheight);

  return {(float)dwidth, (float)dheight};
}

NAPI_FUNCTION(Node_setMeasureFunc) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  global_env = env;
  napi_set_named_property(env, jsThis, "_measureFunc", argv[0]);
  YGNodeSetMeasureFunc(node, &globalMeasureFunc);
  return NULL;
}

NAPI_FUNCTION(Node_unsetMeasureFunc) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  napi_value undefined;
  napi_get_undefined(env, &undefined);
  napi_set_named_property(env, jsThis, "_measureFunc", undefined);
  YGNodeSetMeasureFunc(node, NULL);
  return NULL;
}

static void globalDirtiedFunc(YGNodeConstRef nodeRef) {
  napi_ref ref = (napi_ref)(YGNodeGetContext(nodeRef));

  napi_value jsThis;
  napi_get_reference_value(global_env, ref, &jsThis);

  napi_value dirtiedFunc;
  napi_get_named_property(global_env, jsThis, "_dirtiedFunc", &dirtiedFunc);

  napi_value result;
  napi_call_function(global_env, jsThis, dirtiedFunc, 0, NULL, &result);
}

NAPI_FUNCTION(Node_setDirtiedFunc) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  global_env = env;
  napi_set_named_property(env, jsThis, "_dirtiedFunc", argv[0]);
  YGNodeSetDirtiedFunc(node, &globalDirtiedFunc);
  return NULL;
}

NAPI_FUNCTION(Node_unsetDirtiedFunc) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  napi_value undefined;
  napi_get_undefined(env, &undefined);
  napi_set_named_property(env, jsThis, "_dirtiedFunc", undefined);
  YGNodeSetDirtiedFunc(node, NULL);
  return NULL;
}

NAPI_FUNCTION(Node_markDirty) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGNodeMarkDirty(node);
  return NULL;
}

NAPI_FUNCTION(Node_isDirty) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  bool isDirty = YGNodeIsDirty(node);
  return js_bool(env, isDirty);
}

NAPI_FUNCTION(Node_markLayoutSeen) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  YGNodeSetHasNewLayout(node, false);
  return NULL;
}

NAPI_FUNCTION(Node_hasNewLayout) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  bool hasNewLayout = YGNodeGetHasNewLayout(node);
  return js_bool(env, hasNewLayout);
}

NAPI_FUNCTION(Node_calculateLayout) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 3);
  NAPI_ARG_DOUBLE(width, 0);
  NAPI_ARG_DOUBLE(height, 1);
  NAPI_ARG_INT32(direction, 2);
  YGNodeCalculateLayout(node, width, height,
                        static_cast<YGDirection>(direction));
  return NULL;
}

NAPI_FUNCTION(Node_getComputedLeft) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  float left = YGNodeLayoutGetLeft(node);
  return js_double(env, left);
}

NAPI_FUNCTION(Node_getComputedRight) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  float right = YGNodeLayoutGetRight(node);
  return js_double(env, right);
}

NAPI_FUNCTION(Node_getComputedTop) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  float top = YGNodeLayoutGetTop(node);
  return js_double(env, top);
}

NAPI_FUNCTION(Node_getComputedBottom) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  float bottom = YGNodeLayoutGetBottom(node);
  return js_double(env, bottom);
}

NAPI_FUNCTION(Node_getComputedWidth) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  float width = YGNodeLayoutGetWidth(node);
  return js_double(env, width);
}

NAPI_FUNCTION(Node_getComputedHeight) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  float height = YGNodeLayoutGetHeight(node);
  return js_double(env, height);
}

NAPI_FUNCTION(Node_getComputedLayout) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  napi_value obj;
  napi_create_object(env, &obj);
  napi_set_named_property(env, obj, "left",
                          js_double(env, YGNodeLayoutGetLeft(node)));
  napi_set_named_property(env, obj, "right",
                          js_double(env, YGNodeLayoutGetRight(node)));
  napi_set_named_property(env, obj, "top",
                          js_double(env, YGNodeLayoutGetTop(node)));
  napi_set_named_property(env, obj, "bottom",
                          js_double(env, YGNodeLayoutGetBottom(node)));
  napi_set_named_property(env, obj, "width",
                          js_double(env, YGNodeLayoutGetWidth(node)));
  napi_set_named_property(env, obj, "height",
                          js_double(env, YGNodeLayoutGetHeight(node)));
  return obj;
}

NAPI_FUNCTION(Node_getComputedMargin) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  float margin = YGNodeLayoutGetMargin(node, static_cast<YGEdge>(edge));
  return js_double(env, margin);
}

NAPI_FUNCTION(Node_getComputedBorder) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  float border = YGNodeLayoutGetBorder(node, static_cast<YGEdge>(edge));
  return js_double(env, border);
}

NAPI_FUNCTION(Node_getComputedPadding) {
  NAPI_METHOD_HEADER(YGNodeRef, node, 1);
  NAPI_ARG_INT32(edge, 0);
  float padding = YGNodeLayoutGetPadding(node, static_cast<YGEdge>(edge));
  return js_double(env, padding);
}

NAPI_FUNCTION(Node_getDirection) {
  NAPI_METHOD_HEADER_NO_ARGS(YGNodeRef, node);
  int direction = YGNodeStyleGetDirection(node);
  return js_int32(env, direction);
}

// } /* class Node */

// Setup the classes then export

extern "C" napi_value napi_register_module_v1(napi_env env,
                                              napi_value exports) {
  napi_property_descriptor Config_props[] = {
      NAPI_STATIC_METHOD(Config, create),
      NAPI_STATIC_METHOD(Config, destroy),
      NAPI_METHOD(Config, free),
      NAPI_METHOD(Config, setExperimentalFeatureEnabled),
      NAPI_METHOD(Config, setPointScaleFactor),
      NAPI_METHOD(Config, setErrata),
      NAPI_METHOD(Config, setUseWebDefaults),
      NAPI_METHOD(Config, isExperimentalFeatureEnabled),
      NAPI_METHOD(Config, getErrata),
      NAPI_METHOD(Config, useWebDefaults),
  };

  DEFINE_CLASS(Config, 10);

  napi_property_descriptor Node_props[] = {
      {"create", NULL, Node_createWithConfig, NULL, NULL, NULL, napi_static,
       NULL},
      NAPI_STATIC_METHOD(Node, createDefault),
      NAPI_STATIC_METHOD(Node, createWithConfig),
      NAPI_STATIC_METHOD(Node, destroy),
      NAPI_METHOD(Node, free),
      NAPI_METHOD(Node, freeRecursive),
      NAPI_METHOD(Node, reset),
      NAPI_METHOD(Node, copyStyle),
      NAPI_METHOD(Node, setPositionType),
      NAPI_METHOD(Node, setPosition),
      NAPI_METHOD(Node, setPositionPercent),
      NAPI_METHOD(Node, setAlignContent),
      NAPI_METHOD(Node, setAlignItems),
      NAPI_METHOD(Node, setAlignSelf),
      NAPI_METHOD(Node, setFlexDirection),
      NAPI_METHOD(Node, setFlexWrap),
      NAPI_METHOD(Node, setJustifyContent),
      NAPI_METHOD(Node, setMargin),
      NAPI_METHOD(Node, setMarginPercent),
      NAPI_METHOD(Node, setMarginAuto),
      NAPI_METHOD(Node, setOverflow),
      NAPI_METHOD(Node, setDisplay),
      NAPI_METHOD(Node, setFlex),
      NAPI_METHOD(Node, setFlexBasis),
      NAPI_METHOD(Node, setFlexBasisPercent),
      NAPI_METHOD(Node, setFlexBasisAuto),
      NAPI_METHOD(Node, setFlexGrow),
      NAPI_METHOD(Node, setFlexShrink),
      NAPI_METHOD(Node, setWidth),
      NAPI_METHOD(Node, setWidthPercent),
      NAPI_METHOD(Node, setWidthAuto),
      NAPI_METHOD(Node, setHeight),
      NAPI_METHOD(Node, setHeightPercent),
      NAPI_METHOD(Node, setHeightAuto),
      NAPI_METHOD(Node, setMinWidth),
      NAPI_METHOD(Node, setMinWidthPercent),
      NAPI_METHOD(Node, setMinHeight),
      NAPI_METHOD(Node, setMinHeightPercent),
      NAPI_METHOD(Node, setMaxWidth),
      NAPI_METHOD(Node, setMaxWidthPercent),
      NAPI_METHOD(Node, setMaxHeight),
      NAPI_METHOD(Node, setMaxHeightPercent),
      NAPI_METHOD(Node, setAspectRatio),
      NAPI_METHOD(Node, setBorder),
      NAPI_METHOD(Node, setPadding),
      NAPI_METHOD(Node, setPaddingPercent),
      NAPI_METHOD(Node, setGap),
      NAPI_METHOD(Node, setGapPercent),
      NAPI_METHOD(Node, setDirection),
      NAPI_METHOD(Node, getPositionType),
      NAPI_METHOD(Node, getPosition),
      NAPI_METHOD(Node, getAlignContent),
      NAPI_METHOD(Node, getAlignItems),
      NAPI_METHOD(Node, getAlignSelf),
      NAPI_METHOD(Node, getFlexDirection),
      NAPI_METHOD(Node, getFlexWrap),
      NAPI_METHOD(Node, getJustifyContent),
      NAPI_METHOD(Node, getMargin),
      NAPI_METHOD(Node, getFlexBasis),
      NAPI_METHOD(Node, getFlexGrow),
      NAPI_METHOD(Node, getFlexShrink),
      NAPI_METHOD(Node, getWidth),
      NAPI_METHOD(Node, getHeight),
      NAPI_METHOD(Node, getMinWidth),
      NAPI_METHOD(Node, getMinHeight),
      NAPI_METHOD(Node, getMaxWidth),
      NAPI_METHOD(Node, getMaxHeight),
      NAPI_METHOD(Node, getAspectRatio),
      NAPI_METHOD(Node, getBorder),
      NAPI_METHOD(Node, getOverflow),
      NAPI_METHOD(Node, getDisplay),
      NAPI_METHOD(Node, getPadding),
      NAPI_METHOD(Node, getGap),
      NAPI_METHOD(Node, insertChild),
      NAPI_METHOD(Node, removeChild),
      NAPI_METHOD(Node, getChildCount),
      NAPI_METHOD(Node, getParent),
      NAPI_METHOD(Node, getChild),
      NAPI_METHOD(Node, setAlwaysFormsContainingBlock),
      NAPI_METHOD(Node, isReferenceBaseline),
      NAPI_METHOD(Node, setIsReferenceBaseline),
      NAPI_METHOD(Node, setMeasureFunc),
      NAPI_METHOD(Node, unsetMeasureFunc),
      NAPI_METHOD(Node, setDirtiedFunc),
      NAPI_METHOD(Node, unsetDirtiedFunc),
      NAPI_METHOD(Node, markDirty),
      NAPI_METHOD(Node, isDirty),
      NAPI_METHOD(Node, markLayoutSeen),
      NAPI_METHOD(Node, hasNewLayout),
      NAPI_METHOD(Node, calculateLayout),
      NAPI_METHOD(Node, getComputedLeft),
      NAPI_METHOD(Node, getComputedRight),
      NAPI_METHOD(Node, getComputedTop),
      NAPI_METHOD(Node, getComputedBottom),
      NAPI_METHOD(Node, getComputedWidth),
      NAPI_METHOD(Node, getComputedHeight),
      NAPI_METHOD(Node, getComputedLayout),
      NAPI_METHOD(Node, getComputedMargin),
      NAPI_METHOD(Node, getComputedBorder),
      NAPI_METHOD(Node, getComputedPadding),
      NAPI_METHOD(Node, getDirection),
  };

  DEFINE_CLASS(Node, 101);

  napi_property_descriptor exports_props[] = {
      NAPI_VALUE(Config),
      NAPI_VALUE(Node),
  };

  napi_define_properties(env, exports, 2, exports_props);

  return exports;
}
