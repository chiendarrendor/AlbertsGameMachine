---
name: aws-deployment
description: Current (as-deployed) AWS setup for The Game Machine — EC2 server + CloudFront-hosted client jars
metadata: 
  node_type: memory
  type: project
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-12T09:19:11.900Z
---

Part of [[project-overview]]. Describes the project's *current* production/live deployment on AWS — distinct from the aspirational future serverless migration tracked as [[roadmap-todo]] item #8 (Lambda/API Gateway/DynamoDB). Do not conflate the two.

- The C++ [[component-cpp-server]] currently compiles and runs on an **AWS EC2** instance.
- The Java client jars (see [[component-java-client]]) are hosted via **AWS CloudFront**.
- `awsnotes.txt` (project root — **correction 2026-09-28: this is git-tracked, not untracked**, confirmed via `git status`) contains Albert's working notes from getting this EC2/CloudFront deployment running. Matches git history: commits like "Updating Outpost client for AWS deployment...reorganizing how we handle images" and "bug found in migrating to EC2...Factory Widget used old image name convention" describe this same migration effort.

**How to apply:** When asked about "the AWS deployment" or "how the server currently runs in production," this (EC2 + CloudFront) is the current answer — not the Lambda/serverless idea, which is unstarted future work. `awsnotes.txt` is a legitimate source of deployment detail (ports, image/asset handling quirks, EC2 setup steps) and worth reading if doing anything deployment-related, rather than treating it as disposable scratch.

**Flagged for a future revisit, not yet done (Albert, 2026-09-28):** each per-game/client Makefile's `clientinstall`/`awsinstall` targets (`aws s3 cp ... s3://albertsgamemachine/`) need a real review to confirm the jars/XML actually land where CloudFront expects them internally in S3 — raised while fixing the client Java builds' `.class`-output-location problem (see [[build-environment]]'s "classes/ build-output directory" note), since that touched several of these same install targets' file paths. Not a known bug, just an unverified assumption worth checking before relying on it — the install-path plumbing (`clientinstall`'s `cp`/`jar` steps feeding into `awsinstall`'s `aws s3 cp`) was only checked for "does it read from the right local location," not "does it write to the right S3 location for CloudFront to serve it correctly."
