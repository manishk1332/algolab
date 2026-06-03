# Knowledge Base

Most AI workflows repeatedly solve the same problem:

```text
Task
→ Explain project context
→ Prompt AI
→ Get output
→ Repeat next week
```

This does not scale because project knowledge never accumulates.

A better approach is to build a structured knowledge base that captures project context, design decisions, operational knowledge, and current work. AI agents retrieve from this knowledge base rather than requiring context to be re-explained every session.

The key insight is:

> Context engineering is more valuable than prompt engineering.

A mediocre prompt with excellent project context typically outperforms an excellent prompt with poor context.

---

# Core Principles

## 1. Store Knowledge, Not Conversations

The Knowledge Base should contain durable information:

### Good Candidates

* System architecture
* Design decisions
* Domain knowledge
* Troubleshooting guides
* Team conventions
* Current project status

### Poor Candidates

* Entire chat histories
* Every debugging session
* Every PR description
* Temporary discussions

The objective is a curated knowledge system, not a transcript archive.

---

## 2. Context Should Be Retrieved, Not Injected

Bad:

```text
Load 50 markdown files
Load entire project history
```

Good:

```text
Load index
Find relevant knowledge
Retrieve only what's needed
```

This keeps token usage low while preserving access to large amounts of information.

---

# Architecture

The knowledge system should be separate from the codebase itself.

```text
project/
│
├── <tool bootstrap file>
│
├── knowledge/
│   ├── INDEX.md
│   ├── CURRENT_WORK.md
│   ├── ARCHITECTURE.md
│   │
│   ├── architecture/
│   ├── decisions/
│   ├── troubleshooting/
│   ├── onboarding/
│   └── projects/
│
└── archive/
```

---

# Layer 1: Agent Bootstrap

Every coding agent has its own equivalent:

| Tool        | Bootstrap File |
| ----------- | -------------- |
| Claude Code | CLAUDE.md      |
| Codex       | AGENTS.md      |
| Cline       | .clinerules    |

Only one of these is needed because most developers use a single primary tool.

The bootstrap file is **not the Knowledge Base**.

Its purpose is simply to instruct the agent how to use the knowledge base.

Example:

```md
Read:

1. knowledge/INDEX.md
2. knowledge/CURRENT_WORK.md

Use INDEX.md as the retrieval map.

Load additional files only when relevant.

After completing work:
- Update CURRENT_WORK.md
- Record important decisions
- Update INDEX.md if necessary
```

This file should remain small and mostly static.

---

# Layer 2: Active Context

## INDEX.md

This is the most important file in the system.

Think of it as a retrieval map.

Example:

```md
# Knowledge Index

## Architecture

Auth System
File: architecture/auth.md
Tags: oauth, jwt

Billing System
File: architecture/billing.md
Tags: payments, invoices

## Decisions

ADR-001 PostgreSQL Selection
ADR-002 Cache Strategy

## Troubleshooting

Redis Connection Exhaustion
Kafka Consumer Lag

## Current Priorities

Billing Migration
API Refactor
```

Every session starts here.

---

## CURRENT_WORK.md

Contains active project context.

Example:

```md
# Current Sprint

Goals
- Billing migration
- Redis optimization

Open Issues
- Retry strategy undecided
- API timeout spikes

Recent Decisions
- Cache TTL increased to 15 minutes
```

This bridges long-term memory and day-to-day execution.

---

# Layer 3: Long-Term Memory

This is where durable project knowledge lives.

---

## Architecture Documentation

High-level system maps.

Example:

```md
# Billing Service

Responsibilities
- Invoice generation
- Payment processing
- Subscription management

Dependencies
- PostgreSQL
- Stripe
- Redis

Key Files
- billing_service.py
- invoice_generator.py
```

The purpose is navigation and context retrieval, not exhaustive documentation.

---

## Architecture Decision Records (ADRs)

One of the highest-value knowledge sources.

Example:

```md
# ADR-001

Decision
Use PostgreSQL instead of MongoDB.

Reason
Billing workflows require transactional guarantees.

Alternatives
- MongoDB
- DynamoDB

Date
2026-06-03
```

Months later:

```text
Why did we choose PostgreSQL?
```

The answer is immediately available.

---

## Troubleshooting Knowledge

Capture operational knowledge explicitly.

Example:

```md
# Redis Connection Exhaustion

Symptoms
- Increased latency
- Connection refused errors

Root Cause
Connection pool exhaustion

Resolution
Increase max_connections to 100

Relevant Files
- redis_config.py
- cache_manager.py
```

This becomes invaluable during incidents.

---

# Layer 4: Archive

Completed projects and historical notes belong here.

```text
archive/
├── old_status_reports/
├── completed_projects/
├── meeting_notes/
└── deprecated_designs/
```

Archive information should rarely be loaded directly.

---

# Managing Token Usage

A common concern is context growth.

Bad implementation:

```text
6 months later

100 markdown files
150k tokens
```

Good implementation:

```text
Always load:
- INDEX.md
- CURRENT_WORK.md

Load:
- Relevant files only
```

Example:

Task:

```text
Investigate billing timeout issue
```

Retrieval process:

```text
Read INDEX.md

Find:
- architecture/billing.md
- troubleshooting/redis.md

Load only those files
```

Token usage stays small while the knowledge base can grow indefinitely.

---

# Knowledge Compression

The Knowledge Base should become more condensed over time.

Bad:

```text
January Notes
February Notes
March Notes
April Notes
...
```

Good:

```text
Raw Notes
↓
Summaries
↓
ADRs
Known Issues
Architecture Docs
```

The system should continuously transform information into higher-value forms.

---

# Workflows Built on Top

The knowledge base is the foundation.

Automation is built on top of it.

---

## PR Generation

Inputs:

```text
Git diff
Architecture docs
Relevant ADRs
```

Outputs:

```text
Summary
Testing Notes
Risks
Deployment Considerations
```

---

## Status Reports

Inputs:

```text
CURRENT_WORK.md
Recent commits
```

Outputs:

```text
Completed work
Current blockers
Next milestones
```

---

## Incident Investigation

Inputs:

```text
Error description
Recent changes
Troubleshooting knowledge
```

Outputs:

```text
Likely root causes
Relevant code areas
Suggested debugging steps
```

---

## Knowledge Transfer

Inputs:

```text
Feature name
```

Outputs:

```text
Architecture overview
Important files
Known pitfalls
Design rationale
```

This is particularly useful for onboarding.

---

# Operational Process

The entire system depends on one habit:

At the end of every work session:

```text
1. Update CURRENT_WORK.md
2. Record significant decisions
3. Update architecture docs if needed
4. Update INDEX.md if knowledge was added
5. Archive or summarize obsolete information
```

This typically takes less than a minute and creates compounding value over time.

---

# Key Takeaways

1. Optimize context, not prompts.
2. Treat AI as a continuously learning team member.
3. Keep the bootstrap file (`CLAUDE.md`, `AGENTS.md`, or `.clinerules`) small and stable.
4. Use `INDEX.md` as a retrieval layer rather than loading everything.
5. Store durable knowledge, not conversations.
6. Continuously compress and summarize information.
7. Build workflows (PRs, investigations, status reports, onboarding) on top of the knowledge base.
8. Separate:

   * Agent instructions
   * Active context
   * Long-term knowledge
   * Archived information

The end goal is not a larger prompt. The end goal is a structured knowledge system that allows an AI agent to operate with project-specific context from the moment a session begins.
