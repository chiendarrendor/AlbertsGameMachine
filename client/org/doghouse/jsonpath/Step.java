package org.doghouse.jsonpath;

import java.util.List;

/**
 * The compiled form of a JsonPath expression: a flat sequence of these,
 * built once by PathParser and walked repeatedly by JsonPath.evaluate().
 * Package-private -- callers only ever see JsonPath itself.
 */
sealed interface Step permits FieldStep, IndexStep, MapStep, DictStep, SizeStep {}

/** '.name' or '["name"]' -- object field access by a literal key. The very
 *  first segment of a path is handled by this same step, with no special
 *  treatment -- JsonPath has no notion of a distinguished "root" field. */
record FieldStep(String name) implements Step {}

/** '[N]' -- array access by a literal, fixed position. */
record IndexStep(int index) implements Step {}

/**
 * '[]' -- map. Evaluates `subpath` once per element of the array this step
 * is applied to, collecting the results into a new list. Always consumes
 * the remainder of the path -- nothing can follow a MapStep in the same
 * step list, only inside its own subpath (see PathParser).
 */
record MapStep(List<Step> subpath) implements Step {}

/**
 * '{}' -- dict. The object-keyed analog of MapStep: evaluates `subpath` once
 * per *value* of the object this step is applied to, collecting the results
 * into a new Map<String,Object> keyed by the object's own field names (not a
 * List -- there's no array position to collect into, only the original
 * keys). Always consumes the remainder of the path, same as MapStep -- see
 * PathParser.
 */
record DictStep(List<Step> subpath) implements Step {}

/** '#size' -- element count (array) or key count (object). Always terminal. */
record SizeStep() implements Step {}
