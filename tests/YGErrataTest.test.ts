/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Yoga from "yoga-layout";
import { expect } from "jsr:@std/expect";

Deno.test("errata_all_contains_example_errata", () => {
  const config = Yoga.Config.create();
  config.setErrata(Yoga.ERRATA_ALL);

  expect(config.getErrata()).toBe(Yoga.ERRATA_ALL);
  expect(config.getErrata() & Yoga.ERRATA_STRETCH_FLEX_BASIS).not.toBe(0);

  config.free();
});

Deno.test("errata_none_omits_example_errata", () => {
  const config = Yoga.Config.create();
  config.setErrata(Yoga.ERRATA_NONE);

  expect(config.getErrata()).toBe(Yoga.ERRATA_NONE);
  expect(config.getErrata() & Yoga.ERRATA_STRETCH_FLEX_BASIS).toBe(0);

  config.free();
});

Deno.test("errata_is_settable", () => {
  const config = Yoga.Config.create();

  config.setErrata(Yoga.ERRATA_ALL);
  expect(config.getErrata()).toBe(Yoga.ERRATA_ALL);

  config.setErrata(Yoga.ERRATA_NONE);
  expect(config.getErrata()).toBe(Yoga.ERRATA_NONE);

  config.free();
});
