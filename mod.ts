/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// deno-lint-ignore-file no-explicit-any

import type { Layout, Node, Yoga } from "./src/yoga.ts";
import { constants, Direction, Unit } from "./src/yoga_const.ts";

let module!: { exports: Yoga };

if (typeof Deno === "object") {
  const { dlopen } = await import("node:process");
  module = { exports: {} } as any;
  dlopen(
    module,
    new URL("./build/libyoga_node_api.dylib", import.meta.url).pathname,
  );
  // @ts-expect-error require is not defined
} else if (typeof require !== "undefined") {
  // @ts-expect-error require is not defined
  const r = require;
  module = r(
    "NSBundle" in globalThis
      ? "./Contents/Frameworks/libyoga_node_api.dylib"
      : "./build/libyoga_node_api.dylib",
  );
} else {
  throw new Error("Unhandled runtime");
}

Object.assign(module.exports, constants);

function patch(prototype: any, name: string, fn: any) {
  const original = prototype[name];

  prototype[name] = function (...args: any[]) {
    return fn.call(this, original, ...args);
  };
}

const lib = module.exports as any;

for (
  const fnName of [
    "setPosition",
    "setMargin",
    "setFlexBasis",
    "setWidth",
    "setHeight",
    "setMinWidth",
    "setMinHeight",
    "setMaxWidth",
    "setMaxHeight",
    "setPadding",
    "setGap",
  ]
) {
  // deno-lint-ignore ban-types
  const methods: Record<Unit, Function> = {
    [Unit.Point]: lib.Node.prototype[fnName],
    [Unit.Percent]: lib.Node.prototype[`${fnName}Percent`],
    [Unit.Auto]: lib.Node.prototype[`${fnName}Auto`],
    [Unit.Undefined]: lib.Node.prototype[`${fnName}Auto`],
  };

  patch(
    lib.Node.prototype,
    fnName,
    function (_original: unknown, ...args: any[]) {
      // We patch all these functions to add support for the following calls:
      // .setWidth(100) / .setWidth("100%") / .setWidth(.getWidth()) / .setWidth("auto")

      const value = args.pop();
      let unit: Unit, asNumber;

      if (value === "auto") {
        unit = Unit.Auto;
        asNumber = undefined;
      } else if (typeof value === "object") {
        unit = value.unit;
        asNumber = value.valueOf();
      } else {
        unit = typeof value === "string" && value.endsWith("%")
          ? Unit.Percent
          : Unit.Point;
        asNumber = parseFloat(value);
        if (
          value !== undefined &&
          !Number.isNaN(value) &&
          Number.isNaN(asNumber)
        ) {
          throw new Error(`Invalid value ${value} for ${fnName}`);
        }
      }

      if (!methods[unit as Unit]) {
        throw new Error(
          `Failed to execute "${fnName}": Unsupported unit '${value}'`,
        );
      }

      if (asNumber !== undefined) {
        // @ts-expect-error yeah
        return methods[unit].call(this, ...args, asNumber);
      } else {
        // @ts-expect-error yeah
        return methods[unit].call(this, ...args);
      }
    },
  );
}

patch(
  lib.Node.prototype,
  "calculateLayout",
  function (
    this: Node,
    original: (
      this: Node,
      width: number,
      height: number,
      direction: Direction,
    ) => Layout,
    width = NaN,
    height = NaN,
    direction = Direction.LTR,
  ) {
    // Just a small patch to add support for the function default parameters
    return original.call(this, width, height, direction);
  },
);

function wrapMeasureFunction(
  measureFunction: (...args: any[]) => Layout,
) {
  return (...args: any[]) => {
    const { width, height } = measureFunction(...args);
    return {
      width: width ?? NaN,
      height: height ?? NaN,
    };
  };
}

patch(
  lib.Node.prototype,
  "setMeasureFunc",
  function (
    this: Node,
    original: (
      this: Node,
      measureFunc: (width: number, height: number) => Layout,
    ) => void,
    measureFunc: (width: number, height: number) => Layout,
  ) {
    return original.call(this, wrapMeasureFunction(measureFunc) as any);
  },
);

// hack: for tests
Object.defineProperty(globalThis, "YGBENCHMARK", {
  get: () => Deno.test,
});

export * from "./src/yoga_const.ts";
export * from "./src/yoga.ts";

export default module.exports;
