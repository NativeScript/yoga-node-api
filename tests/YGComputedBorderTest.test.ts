/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from "yoga-layout";
import { expect } from "jsr:@std/expect";

Deno.test("border_start", () => {
  const root = Yoga.Node.create();
  root.setWidth(100);
  root.setHeight(100);
  root.setBorder(Yoga.EDGE_START, 10);

  root.calculateLayout(100, 100, Yoga.DIRECTION_LTR);

  expect(root.getComputedBorder(Yoga.EDGE_LEFT)).toBe(10);
  expect(root.getComputedBorder(Yoga.EDGE_RIGHT)).toBe(0);

  root.calculateLayout(100, 100, Yoga.DIRECTION_RTL);

  expect(root.getComputedBorder(Yoga.EDGE_LEFT)).toBe(0);
  expect(root.getComputedBorder(Yoga.EDGE_RIGHT)).toBe(10);

  root.freeRecursive();
});
