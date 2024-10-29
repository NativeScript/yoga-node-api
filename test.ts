import Yoga from "yoga-layout";

const config = Yoga.Config.create();
console.log(config);

console.log(config.getErrata());
console.log(
  config.isExperimentalFeatureEnabled(Yoga.EXPERIMENTAL_FEATURE_WEB_FLEX_BASIS),
);
console.log(config.useWebDefaults());

config.setExperimentalFeatureEnabled(
  Yoga.EXPERIMENTAL_FEATURE_WEB_FLEX_BASIS,
  true,
);

console.log(
  config.isExperimentalFeatureEnabled(Yoga.EXPERIMENTAL_FEATURE_WEB_FLEX_BASIS),
);

const div = Yoga.Node.create(config);

console.log(div);

div.setWidth(100);
div.setHeight(100);

div.calculateLayout(100, 100);

console.log(div.getComputedLayout());

Yoga.Config.destroy(config);
