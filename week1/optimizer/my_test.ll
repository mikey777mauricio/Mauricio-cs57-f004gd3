; ModuleID = 'test.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  store i32 10, i32* %2, align 4
  %8 = load i32, i32* %2, align 4
  %9 = add nsw i32 %8, 10
  store i32 %9, i32* %2, align 4
  %10 = add nsw i32 %8, %8
  store i32 %10, i32* %2, align 4
  store i32 30, i32* %2, align 4
  store i32 10, i32* %2, align 4
  br label %11

11:                                               ; preds = %14, %1
  %12 = load i32, i32* %2, align 4
  %13 = icmp slt i32 %12, 100
  br i1 %13, label %14, label %18

14:                                               ; preds = %11
  %15 = load i32, i32* %2, align 4
  %16 = add nsw i32 %15, 5
  store i32 %16, i32* %2, align 4
  %17 = add nsw i32 %15, 10
  store i32 %17, i32* %2, align 4
  store i32 %17, i32* %2, align 4
  br label %11, !llvm.loop !6

18:                                               ; preds = %11
  %19 = load i32, i32* %2, align 4
  %20 = icmp sgt i32 %19, 100
  br i1 %20, label %21, label %23

21:                                               ; preds = %18
  %22 = load i32, i32* %2, align 4
  store i32 %22, i32* %2, align 4
  br label %24

23:                                               ; preds = %18
  store i32 100, i32* %2, align 4
  br label %24

24:                                               ; preds = %23, %21
  %25 = load i32, i32* %2, align 4
  ret i32 %25
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
