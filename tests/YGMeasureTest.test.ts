/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from "yoga-layout";
import { expect } from "jsr:@std/expect";
import { getMeasureCounter } from "./tools/MeasureCounter.ts";

Deno.test("dont_measure_single_grow_shrink_child", () => {
  const root = Yoga.Node.create();
  root.setWidth(100);
  root.setHeight(100);

  const measureCounter = getMeasureCounter(null, 100, 100);

  const root_child0 = Yoga.Node.create();
  root_child0.setMeasureFunc(measureCounter.inc);
  root_child0.setFlexGrow(1);
  root_child0.setFlexShrink(1);
  root.insertChild(root_child0, 0);
  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  expect(measureCounter.get()).toBe(0);

  root.freeRecursive();
});

Deno.test("dont_fail_with_incomplete_measure_dimensions", () => {
  // @ts-expect-error Testing bad usage
  const heightOnlyCallback = getMeasureCounter(() => ({ height: 10 }));
  // @ts-expect-error Testing bad usage
  const widthOnlyCallback = getMeasureCounter(() => ({ width: 10 }));
  // @ts-expect-error Testing bad usage
  const emptyCallback = getMeasureCounter(() => ({}));

  const root = Yoga.Node.create();
  root.setWidth(100);
  root.setHeight(100);

  const node1 = Yoga.Node.create();
  const node2 = Yoga.Node.create();
  const node3 = Yoga.Node.create();

  root.insertChild(node1, root.getChildCount());
  root.insertChild(node2, root.getChildCount());
  root.insertChild(node3, root.getChildCount());

  node1.setMeasureFunc(heightOnlyCallback.inc);
  node2.setMeasureFunc(widthOnlyCallback.inc);
  node3.setMeasureFunc(emptyCallback.inc);

  root.calculateLayout(undefined, undefined, Yoga.DIRECTION_LTR);

  expect(heightOnlyCallback.get()).toBe(1);
  expect(widthOnlyCallback.get()).toBe(1);
  expect(emptyCallback.get()).toBe(1);

  expect(node1.getComputedWidth()).toBe(100);
  expect(node1.getComputedHeight()).toBe(10);

  expect(node2.getComputedWidth()).toBe(100);
  expect(node2.getComputedHeight()).toBe(0);

  expect(node3.getComputedWidth()).toBe(100);
  expect(node3.getComputedHeight()).toBe(0);

  root.freeRecursive();
});
